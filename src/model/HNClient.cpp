#include "HNClient.hpp"
#include <curl/curl.h>
#include <stdlib.h>
#include <iostream>


static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

HNClient::HNClient(){
   CURLcode code =  curl_global_init(CURL_GLOBAL_ALL);
   if(code!=CURLE_OK){
        // TODO make personalized errors
        throw std::runtime_error("ERROR: happened during the set up of curl");
   }
}

HNClient::~HNClient(){

}

std::vector<long> HNClient::FetchToStories(){
    this->handle = curl_easy_init();
    std::vector<long> top_story_id_list;
    std::string result_sting = "";

    if(handle==nullptr)
        // TODO make better error handling
        throw std::runtime_error("ERROR: during the initialization of curl");

    curl_easy_setopt(handle, CURLOPT_URL, this->top_story_url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &result_sting);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15L);

    CURLcode result = curl_easy_perform(handle);
    if(result!=CURLE_OK)
        throw std::runtime_error("asdasdasds");

    curl_easy_cleanup(handle);
   std::cout << result_sting << std::endl;
  

    return top_story_id_list;
}


// nlohmann::json HNClient::FetchStory(long id){
//     
// }



