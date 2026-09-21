#include "app_model.hpp"
#include "lexbor/dom/interface.h"
#include "model/ArticleScraper.hpp"
#include <iostream>
#include <thread>

AppModel::AppModel()
{
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

StoryDataBlock AppModel::FetchStory(ID id)
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

CommentDataBlock AppModel::FetchComment(ID id)
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

JobDataBlock AppModel::FetchJob(ID id)
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

void AppModel::GetArticle(std::string articleUrl)
{
   if(client == nullptr)
      return;



   articleContent_.store(nullptr);
   std::thread([self = shared_from_this(), articleUrl]()
   {
      auto article = self->client->FetchArticle(articleUrl);
      self->articleContent_.store(std::make_shared<ContentBlock>((*self->scraper_)(article)));
   }).detach();
}

void AppModel::LoadMoreStories()
{
   bool expected = false;
   if(!is_loading_.compare_exchange_strong(expected, true))
      return;

   std::thread([self = shared_from_this()]()
   {
      try{
         for(int i = 0; i < self->fetch_limit; i++)
         {
            auto combined = std::make_shared<std::vector<StoryDataBlock>>(*self->articles_list_.load());
            combined->push_back(self->FetchStory(self->top_stories_.SafeNext()));
            self->articles_list_.store(combined);
         }
      }
      catch(const std::exception& e){
         std::cerr << "Error: " << e.what() << std::endl;
      }

      self->is_loading_.store(false);
   }).detach();
}
