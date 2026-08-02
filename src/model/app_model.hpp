#ifndef APP_MODEL_H
#define APP_MODEL_H

#include <vector>
#include <string>
#include "HNClient.hpp"
#include "model/DataBlock.hpp"

// Class to hold the state of the shown IDs window
class ShownIdWindow{
    public:
        ShownIdWindow() = default;
        ShownIdWindow(std::vector<long> id_list){
            ids = std::move(id_list);
        }
        ~ShownIdWindow() = default;
        int GetLastShownIndex() const { return LastShownIndex; }
        bool HasNext() const { return GetLastShownIndex() < ids.size();}
        void RefreshWindow(std::vector<long> new_ids) {
            LastShownIndex = -1;
            ids = std::move(new_ids);
        }

    private:
        int LastShownIndex = -1;
        std::vector<long> ids;
};


class AppModel {
    public:
        AppModel();
        ~AppModel();

        std::vector<ID> FetchTopStories();
        void FetchStory(ID id);
        void FetchComment(ID id);
        void FetchJob(ID id);
        std::shared_ptr<std::vector<JobDataBlock>> GetJobList() const { return job_list_; }
        std::shared_ptr<std::vector<CommentDataBlock>> GetComments() const { return comments_; }
        std::shared_ptr<std::vector<ArticleDataBlock>> GetArticlesList() const { return articles_list_; }

        int selected = 0;
        std::vector<std::string> news_list;

    private:
        
        HNClient* client = nullptr;
        ShownIdWindow* shown_ids;
        std::shared_ptr<std::vector<JobDataBlock>> job_list_;
        std::shared_ptr<std::vector<CommentDataBlock>> comments_;
        std::shared_ptr<std::vector<ArticleDataBlock>> articles_list_;
};


#endif // APP_MODEL_H