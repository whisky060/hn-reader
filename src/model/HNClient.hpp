#ifndef HNCLIENT_H
#define HMCLIENT_H

#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include <curl/curl.h>



using namespace nlohmann;


class HNClient {
    public:
        HNClient();
        ~HNClient();
        std::vector<long> FetchToStories();
        nlohmann::json FetchStory(long id);
        
    private:
        std::vector<long> CreateIdList(const std::string& json_data);

        CURL* handle = nullptr;
        const char* top_story_url = "https://hacker-news.firebaseio.com/v0/topstories.json\0";
        const char* story_url_template = "https://hacker-news.firebaseio.com/v0/item/{id}.json?print=pretty";
 
};



#endif