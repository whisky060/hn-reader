#include "LruCache.hpp"
#include <mutex>
#include <optional>
#include <string>


std::optional<std::string> LruCache::get(const std::string key)
{
  std::lock_guard<std::mutex> lk(m_);
  auto it  = index_.find(key);
  if( it == index_.end())
    return std::nullopt;

  items_.splice(items_.begin(), items_, it->second);

  return it->second->second;
}

void LruCache::put(const std::string key, std::string value)
{
  std::lock_guard<std::mutex> lk(m_);
  auto it =  index_.find(key);

  // already in the cache
  if(it != index_.end())
  {
    it->second->second = std::move(value);
    items_.splice(items_.begin(), items_, it->second);
    return;
  }

  items_.emplace_front(key, std::move(value));
  index_[key] = items_.begin();
  if(items_.size() > capacity_)
  {
    index_.erase(items_.back().first);
    items_.pop_back();
  }
}
