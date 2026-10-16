#include "app_model.hpp"
#include "concurrency/Dispatcher.hpp"
#include "concurrency/FetchResult.hpp"
#include "concurrency/ThreadPool.hpp"
#include "model/ArticleScraper.hpp"
#include "model/ContentBlock.hpp"
#include "model/DataBlock.hpp"
#include "exeptions/scraping_exception.hpp"
#include "utility/utility.h"
#include <exception>
#include <memory>
#include <string>
#include <vector>

AppModel::AppModel(std::shared_ptr<Dispatcher> dispatcher)
{
   dispatcher_ = std::move(dispatcher);
   pool_ = std::make_shared<ThreadPool>();
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
   auto parser = utils::PlainHTMLPArsing();
   nlohmann::json comment = client->FetchID(id);
   auto text  = comment.value("text", "");
   auto comment_block = CommentDataBlock(
       DataBlockType::Comment,
       comment.value("by", ""),
       comment.value("id", 0),
       comment.value("kids", std::vector<ID>{}),
       comment.value("parent", 0),
       comment.value("text", ""),
       comment.value("time", 0),
       parser(text)
   );

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


  auto fn = pool_->Submit([self = shared_from_this(),articleUrl, callback](){
    try
    {
      auto article = self->client->FetchArticle(articleUrl);
      auto content =  (*self->scraper_)(article);
      auto res = FetchResult<ContentBlock>::Success(content);
      self->dispatcher_->Post([callback,res](){ callback(res); });
    } catch (const ScrapingException& e) {
      auto url = e.GetUrl().empty() ? articleUrl : e.GetUrl();
      auto res = FetchResult<ContentBlock>::Failure(
          std::make_exception_ptr(ScrapingException(e.what(), url)));
      self->dispatcher_->Post([callback,res](){ callback(res);});
    } catch (...) {
      auto res = FetchResult<ContentBlock>::Failure(std::current_exception());
      self->dispatcher_->Post([callback,res](){ callback(res);});
    }
  });
}

void AppModel::FetchMoreStories(ListCallback callback)
{
  auto fn = pool_->Submit([self = shared_from_this(), callback]()
    {
      try {
        std::vector<StoryDataBlock> combined;
        for(int i = 0; i < self->fetch_limit; i++)
        {
          combined.push_back(self->FetchStorySync(self->top_stories_.SafeNext()));
        }
        auto res = FetchResult<std::vector<StoryDataBlock>>::Success(combined);
        self->dispatcher_->Post([callback, res](){callback(res);});
      } catch (...)
      {
        auto res = FetchResult<std::vector<StoryDataBlock>>::Failure(std::current_exception());
        self->dispatcher_->Post([callback,res](){ callback(res);});
      }
    });
}


void AppModel::FetchComments(std::vector<ID> comments, CommentCallback callback)
{
  auto fn = pool_->Submit([self = shared_from_this(), callback, comments]()
    {
      try
      {
        std::vector<CommentDataBlock> commentBlock;
        for(auto& id : comments)
        {
          commentBlock.push_back(self->FetchCommentSync(id));
        }
        auto res = FetchResult<std::vector<CommentDataBlock>>::Success(commentBlock);
        callback(res);
      } catch (...)
      {
        auto res = FetchResult<std::vector<CommentDataBlock>>::Failure(std::current_exception());
        self->dispatcher_->Post([callback, res](){callback(res);});
      }

    });
}
