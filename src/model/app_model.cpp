#include "app_model.hpp"

AppModel::AppModel()
{
   this->news_list = {
       "entry 1",
       "entry 2",
       "entry 3",
       "entry 1",
       "entry 2",
       "entry 3",
       "entry 1",
       "entry 2",
       "entry 3",
       "entry 1",
       "entry 2",
       "entry 3",
       "entry 1",
       "entry 2",
       "entry 3",
   };

   client = new HNClient();
   auto story_ids = FetchTopStories();
   for(auto& id : story_ids){
      FetchStory(id);
   }
}

AppModel::~AppModel()
{
   delete client;
   delete shown_ids;
}

std::vector<ID> AppModel::FetchTopStories()
{
   return std::move(client->FetchToStories());
}

void AppModel::FetchStory(ID id)
{
   nlohmann::json story = client->FetchID(id);

   auto article_block = ArticleDataBlock(
       DataBlockType::Story,
       story["by"].get<std::string>(),
       story["descendants"].get<unsigned long>(),
       story["id"].get<ID>(),
       story["kids"].get<std::vector<ID>>(),
       story["score"].get<int>(),
       story["time"].get<unsigned long>(),
       story["title"].get<std::string>(),
       story["url"].get<std::string>());

   this->articles_list_->push_back(article_block);
}

void AppModel::FetchComment(ID id)
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

   this->comments_->push_back(comment_block);
}

void AppModel::FetchJob(ID id)
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

   this->job_list_->push_back(job_block);
}
