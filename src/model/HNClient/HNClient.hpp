#ifndef HNCLIENT_H
#define HNCLIENT_H

#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include <curl/curl.h>
#include "exeptions/scraping_exception.hpp"
#include "model/Cache/LruCache.hpp"



using namespace nlohmann;
using ID = unsigned long long;

class HNClient {
    public:
        HNClient();
        ~HNClient();
        std::vector<ID> FetchToStories();
        nlohmann::json FetchID(ID id);
        std::string FetchArticle(std::string article_url);

    private:
        std::vector<ID> CreateIdList(const std::string& json_data);
        const char* top_story_url = "https://hacker-news.firebaseio.com/v0/topstories.json\0";
        std::shared_ptr<LruCache> cache_;


};



#endif
