#ifndef INDEXTRACKER_HPP
#define INDEXTRACKER_HPP
#include <vector>
#include <mutex>
#include <utility>


class IndexTracker
{
private:
    using ID = unsigned long long;
    int curr_indx_;
    int max_indx_;
    std::vector<ID> ids_;
    mutable std::mutex mtx_;
    
public:
    IndexTracker() = default;
    IndexTracker(std::vector<ID>& ids): curr_indx_(0), max_indx_(ids.size()-1) { ids_= move(ids); };
    IndexTracker(IndexTracker&& other) noexcept
        : curr_indx_(other.curr_indx_), max_indx_(other.max_indx_), ids_(std::move(other.ids_)) {}
    IndexTracker& operator=(IndexTracker&& other) noexcept
    {
        if (this != &other) {
            std::lock_guard<std::mutex> lock(mtx_);
            curr_indx_ = other.curr_indx_;
            max_indx_ = other.max_indx_;
            ids_ = std::move(other.ids_);
        }
        return *this;
    }
    ~IndexTracker() = default;
    int GetCurrentIndx() const;
    int GetMaxIndx() const;
    ID SafeNext();
    
    
 
};




#endif