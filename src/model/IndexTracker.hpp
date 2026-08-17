#ifndef INDEXTRACKER_HPP
#define INDEXTRACKER_HPP
#include <vector>


class IndexTracker
{
private:
    using ID = unsigned long long;
    int curr_indx_;
    int max_indx_;
    std::vector<ID> ids_;
    
public:
    IndexTracker() = default;
    IndexTracker(std::vector<ID>& ids): curr_indx_(0), max_indx_(ids.size()-1) { ids_= move(ids); };
    ~IndexTracker() = default;
    int GetCurrentIndx() const { return curr_indx_; };
    int GetMaxIndx() const { return max_indx_; };
    ID SafeNext();
    
    

};




#endif