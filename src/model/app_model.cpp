#include "app_model.hpp"
#include "concurrency/FetchResult.hpp"
#include "concurrency/ThreadPool.hpp"
#include "model/ArticleScraper.hpp"
#include "model/DataBlock.hpp"
#include <exception>
#include <memory>
#include <string>
#include <thread>
#include <vector>

AppModel::AppModel()
{
   pool = std::make_shared<ThreadPool>();
   articles_list_.store(std::make_shared<std::vector<StoryDataBlock>>());
   articleContent_ = nullptr;
   comments_ = std::make_shared<std::vector<CommentDataBlock>>();
   job_list_ = std::make_shared<std::vector<JobDataBlock>>();
   auto tmp = FetchTopStories();
   top_stories_ = IndexTracker(tmp);
   scraper_ = std::make_unique<ArticleScraper>();
}



std::vector<ID> AppModel::FetchTopStories()
{
   return std::move(client->FetchToStories());
}

StoryDataBlock AppModel::FetchStorySync(ID id)
{
   nlohmann::json story = client->FetchID(id);

   auto article_block = StoryDataBlock(
       DataBlockType::Story,
       story.value("by", ""),
       story.value("descendants", 0),
       story.value("id", 0),
       story.value("kids", std::vector<ID>{}),
       story.value("score", 0),
       story.value("time", 0),
       story.value("title", ""),
       story.value("url", ""));

   return article_block;
}

CommentDataBlock AppModel::FetchCommentSync(ID id)
{
   nlohmann::json comment = client->FetchID(id);
   auto comment_block = CommentDataBlock(
       DataBlockType::Comment,
       comment.value("by", ""),
       comment.value("id", 0),
       comment.value("kids", std::vector<ID>{}),
       comment.value("parent", 0),
       comment.value("text", ""),
       comment.value("time", 0));

   return comment_block;
}

JobDataBlock AppModel::FetchJobSync(ID id)
{
   nlohmann::json job = client->FetchID(id);
   auto job_block = JobDataBlock(
      DataBlockType::Job,
      job.value("by", ""),
      job.value("id", 0),
      job.value("score", 0),
      job.value("time", 0),
      job.value("title", ""),
      job.value("url", "")
   );

   return job_block;
}

void AppModel::FetchArticle(std::string articleUrl, ArticleCallback callback)
{
  if(client == nullptr)
    return;


  auto fn = pool->Submit([self = shared_from_this(),articleUrl, callback](){
    try
    {
      auto article = self->client->FetchArticle(articleUrl);
      auto res = FetchResult<std::string>::Success(article);
      self->dispatcher->Post([callback,res](){ callback(res); });
    } catch (...) {
      auto res = FetchResult<std::string>::Failure(std::current_exception());
      self->dispatcher->Post([callback,res](){ callback(res);});
    }
  });
}

void AppModel::FetchMoreStories(ListCallback<std::vector<StoryDataBlock>> callback)
{
  auto fn = pool->Submit([self = shared_from_this(), callback]()
    {
      try {
        std::vector<StoryDataBlock> combined;
        for(int i = 0; i < self->fetch_limit; i++)
        {
          combined.push_back(self->FetchStorySync(self->top_stories_.SafeNext()));
        }
        auto res = FetchResult<std::vector<StoryDataBlock>>::Success(combined);
        self->dispatcher->Post([callback, res](){callback(res);});
      } catch (...)
      {
        auto res = FetchResult<std::vector<StoryDataBlock>>::Failure(std::current_exception());
        self->dispatcher->Post([callback,res](){ callback(res);});
      }
    });
}

void AppModel::LoadMoreStories()
{
   bool expected = false;
   if(!is_loading_.compare_exchange_strong(expected, true))
      return;

   std::thread([self = shared_from_this()]()
   {
      try{
          auto combined = std::make_shared<std::vector<StoryDataBlock>>(*self->articles_list_.load());
          for(int i = 0; i < self->fetch_limit; i++)
          {
            combined->push_back(self->FetchStory(self->top_stories_.SafeNext()));
          }
          self->articles_list_.store(combined);
      }
      catch(...){
         self->is_loading_.store(false);
      }

      self->is_loading_.store(false);
   }).detach();
}
