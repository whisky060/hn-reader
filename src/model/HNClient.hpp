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
        //nlohmann::json FetchStory(long id);
        
    private:
        CURL* handle = nullptr;
        const std::string top_story_url = "https://hacker-news.firebaseio.com/v0/topstories.json";
        
 
};



#endif