#ifndef LRUCACHE_HPP
#define LRUCACHE_HPP


#include <cstddef>
#include <list>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>


class LruCache
{

  public:
    LruCache(std::size_t capacity) : capacity_(capacity) {};
    ~LruCache() = default;
    std::optional<std::string> get(const std::string key);
    void put(const std::string key, std::string value);


  private:
    std::mutex m_;
    std::size_t capacity_;
    std::unordered_map<std::string,std::list<std::pair<std::string, std::string>>::iterator> index_;
    std::list<std::pair<std::string, std::string>> items_;

};


#endif
