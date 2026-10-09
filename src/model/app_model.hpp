#ifndef APP_MODEL_H
#define APP_MODEL_H

#include <ftxui/component/app.hpp>
#include <functional>
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
#include "concurrency/FetchResult.hpp"

template<class T>
using CommentCallback = std::function<void(FetchResult<T>)>;


using ArticleCallback = std::function<void(FetchResult<std::string>)>;

template<class T>
using ListCallback = std::function<void(FetchResult<T>)>;

class AppModel : public std::enable_shared_from_this<AppModel> {
    public:
        AppModel();
        ~AppModel()=default;

        std::vector<ID> FetchTopStories();
        StoryDataBlock FetchStorySync(ID id);
        CommentDataBlock FetchCommentSync(ID id);
        JobDataBlock FetchJobSync(ID id);
        void FetchArticle(std::string articleUrl, ArticleCallback callback);
        void FetchMoreStories(ListCallback<std::vector<StoryDataBlock>> callback);


        void LoadMoreStories();


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
