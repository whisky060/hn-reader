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
    curl_global_cleanup();
}

std::vector<long> HNClient::FetchToStories(){
    this->handle = curl_easy_init();
    std::string result_sting = "";

    if(handle==nullptr)
        // TODO make better error handling
        throw std::runtime_error("ERROR: during the initialization of curl");
        
    CURLcode result;
    result = curl_easy_setopt(handle, CURLOPT_URL, this->top_story_url);
    if(result!=CURLE_OK)
        throw std::runtime_error("Error in setting CURLOPT_URL");

    curl_easy_setopt(handle, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
    result = curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
    if(result!=CURLE_OK){
        curl_easy_cleanup(handle);
        throw std::runtime_error("Error in setting CURLOPT_WRITEFUNCTION" + std::string(curl_easy_strerror(result)));
    }

    result = curl_easy_setopt(handle, CURLOPT_WRITEDATA, &result_sting);
    if(result!=CURLE_OK){
        curl_easy_cleanup(handle);
        throw std::runtime_error("Error in setting CURLOPT_WRITEDATA" + std::string(curl_easy_strerror(result)));
    }
        
    result = curl_easy_setopt(handle, CURLOPT_TIMEOUT, 100L);
    if(result!=CURLE_OK){
        curl_easy_cleanup(handle);
        throw std::runtime_error("Error in setting CURLOPT_TIMEOUT" + std::string(curl_easy_strerror(result)));
    }

    result = curl_easy_perform(handle);
    if(result!=CURLE_OK){
        curl_easy_cleanup(handle);
        throw std::runtime_error("Error in fetching data from Hacker News: " + std::string(curl_easy_strerror(result)));
    }

    curl_easy_cleanup(handle);
    std::cout << result_sting << std::endl;
    std::vector<long> id_list = std::move(CreateIdList(result_sting));
    
    return id_list;
}

std::vector<long> HNClient::CreateIdList(const std::string& json_data) {
    nlohmann::json parsed_json = nlohmann::json::parse(json_data);
    std::vector<long> id_list;
    for(const auto& id : parsed_json){
        id_list.push_back(id.get<long>());  
    }
  
    return id_list;
}


 nlohmann::json HNClient::FetchID(long id){
    this->handle = curl_easy_init();
    std::string result_sting = "";

    if(handle==nullptr)
        // TODO make better error handling
        throw std::runtime_error("ERROR: during the initialization of curl");

    CURLcode result;
    result = curl_easy_setopt(handle, CURLOPT_URL, (const char*)((std::string)"https://hacker-news.firebaseio.com/v0/item/" + std::to_string(id) + ".json?print=pretty\0").c_str());
    if(result!=CURLE_OK)
        throw std::runtime_error("Error in setting CURLOPT_URL");

   
    result = curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
    if(result!=CURLE_OK){
        curl_easy_cleanup(handle);
        throw std::runtime_error("Error in setting CURLOPT_WRITEFUNCTION" + std::string(curl_easy_strerror(result)));
    }

    result = curl_easy_setopt(handle, CURLOPT_WRITEDATA, &result_sting);
    if(result!=CURLE_OK){
        curl_easy_cleanup(handle);
        throw std::runtime_error("Error in setting CURLOPT_WRITEDATA" + std::string(curl_easy_strerror(result)));
    }
        
    result = curl_easy_setopt(handle, CURLOPT_TIMEOUT, 100L);
    if(result!=CURLE_OK){
        curl_easy_cleanup(handle);
        throw std::runtime_error("Error in setting CURLOPT_TIMEOUT" + std::string(curl_easy_strerror(result)));
    }

    result = curl_easy_perform(handle);
    if(result!=CURLE_OK){
        curl_easy_cleanup(handle);
        throw std::runtime_error("Error in fetching data from Hacker News: " + std::string(curl_easy_strerror(result)));
    }

    curl_easy_cleanup(handle);

    return nlohmann::json::parse(result_sting);
 }



