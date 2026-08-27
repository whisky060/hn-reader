#ifndef APP_MODEL_H
#define APP_MODEL_H

#include <vector>
#include <string>
#include <atomic>
#include "HNClient.hpp"
#include "model/DataBlock.hpp"
#include "IndexTracker.hpp"


class AppModel {
    public:
        AppModel();
        ~AppModel();

        std::vector<ID> FetchTopStories();
        StoryDataBlock FetchStory(ID id);
        CommentDataBlock FetchComment(ID id);
        JobDataBlock FetchJob(ID id);
        bool isLoading() const {return is_loading_.load();}
        void LoadMoreStories();
        std::shared_ptr<std::vector<JobDataBlock>> GetJobList() const { return job_list_; }
        std::shared_ptr<std::vector<CommentDataBlock>> GetComments() const { return comments_; }
        std::shared_ptr<std::vector<StoryDataBlock>> GetArticlesList() const { return articles_list_; }

    private:
        
        HNClient* client = nullptr;
        const int fetch_limit = 25;
        IndexTracker top_stories_;
        std::shared_ptr<std::vector<JobDataBlock>> job_list_;
        std::shared_ptr<std::vector<CommentDataBlock>> comments_;
        std::shared_ptr<std::vector<StoryDataBlock>> articles_list_;
        std::atomic<bool> is_loading_{false};
};


#endif // APP_MODEL_H