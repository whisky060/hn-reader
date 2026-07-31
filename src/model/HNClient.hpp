#ifndef HNCLIENT_H
#define HMCLIENT_H

#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include <curl/curl.h>



using namespace nlohmann;
using ID = unsigned long long;

class HNClient {
    public:
        HNClient();
        ~HNClient();
        std::vector<ID> FetchToStories();
        nlohmann::json FetchID(ID id);
        
    private:
        std::vector<ID> CreateIdList(const std::string& json_data);

        CURL* handle = nullptr;
        const char* top_story_url = "https://hacker-news.firebaseio.com/v0/topstories.json\0";
        
 
};



#endif