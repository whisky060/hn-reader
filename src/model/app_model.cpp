#include "app_model.hpp"
#include <stdexcept>
#include <iostream>
#include <future>
#include <thread>

AppModel::AppModel()
{
   articles_list_ = std::make_shared<std::vector<StoryDataBlock>>();
   comments_ = std::make_shared<std::vector<CommentDataBlock>>();
   job_list_ = std::make_shared<std::vector<JobDataBlock>>();
   client = new HNClient();
   auto tmp = FetchTopStories();
   top_stories_ = IndexTracker(tmp);
   
   try{

      LoadMoreStories();
   
   }
   catch(const std::out_of_range& e){
      std::cerr << "Error: " << e.what() << std::endl;
   }
}

AppModel::~AppModel()
{
   delete client;
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


void AppModel::LoadMoreStories()
{
   
   try{
      std::thread([this]
         {
            this->is_loading_.exchange(true);
            for(int i = 0; i < this->fetch_limit; i++)
            {
               this->articles_list_->push_back(FetchStory(this->top_stories_.SafeNext()));
               
            }
            this->is_loading_.exchange(false);

      }).detach();

   }
   catch(const std::out_of_range& e){
      std::cerr << "Error: " << e.what() << std::endl;
   }
}