#ifndef APP_MODEL_H
#define APP_MODEL_H

#include <vector>
#include <string>
#include "HNClient.hpp"
#include "model/DataBlock.hpp"
#include "IndexTracker.hpp"


class AppModel {
    public:
        AppModel();
        ~AppModel();

        std::vector<ID> FetchTopStories();
        void FetchStory(ID id);
        void FetchComment(ID id);
        void FetchJob(ID id);
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
};


#endif // APP_MODEL_H