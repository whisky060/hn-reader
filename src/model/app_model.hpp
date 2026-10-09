#ifndef APP_MODEL_H
#define APP_MODEL_H

#include <ftxui/component/app.hpp>
#include <vector>
#include <string>
#include <atomic>
#include <memory>
#include <exception>
#include <mutex>
#include "HNClient.hpp"
#include "model/DataBlock.hpp"
#include "IndexTracker.hpp"
#include "model/ArticleScraper.hpp"
#include "model/ContentBlock.hpp"
#include "concurrency/Dispatcher.hpp"
#include "concurrency/ThreadPool.hpp"

class AppModel : public std::enable_shared_from_this<AppModel> {
    public:
        AppModel();
        ~AppModel()=default;

        std::vector<ID> FetchTopStories();
        StoryDataBlock FetchStory(ID id);
        CommentDataBlock FetchComment(ID id);
        JobDataBlock FetchJob(ID id);
        bool isLoading() const {return is_loading_.load();}
        void LoadMoreStories();
        void GetArticle(std::string articleUrl);
        std::shared_ptr<std::vector<JobDataBlock>> GetJobList() const { return job_list_; }
        std::shared_ptr<std::vector<CommentDataBlock>> GetComments() const { return comments_; }
        std::shared_ptr<const std::vector<StoryDataBlock>> GetArticlesList() const { return articles_list_.load(); }
        std::shared_ptr<ContentBlock> GetArticleContent() { return articleContent_.load(); };
        void SetPendingException(std::exception_ptr e)
        {
            std::lock_guard<std::mutex> lock(exceptionGuard);
            pendingExeption = e;
        };
        std::exception_ptr GetPendingException()
        {
            std::lock_guard<std::mutex> lock(exceptionGuard);
            auto e = pendingExeption;
            pendingExeption = nullptr;
            return e;
        };
    private:

        std::shared_ptr<HNClient> client = std::make_shared<HNClient>();
        std::shared_ptr<Dispatcher> dispatcher;
        std::shared_ptr<ThreadPool> pool;
        std::unique_ptr<ArticleScraper> scraper_;
        const int fetch_limit = 25;
        IndexTracker top_stories_;
        std::atomic<std::shared_ptr<ContentBlock>> articleContent_;
        std::shared_ptr<std::vector<JobDataBlock>> job_list_;
        std::shared_ptr<std::vector<CommentDataBlock>> comments_;
        std::atomic<std::shared_ptr<const std::vector<StoryDataBlock>>> articles_list_;
        std::atomic<bool> is_loading_{false};
        std::exception_ptr pendingExeption;
        std::mutex exceptionGuard;

};


#endif // APP_MODEL_H
