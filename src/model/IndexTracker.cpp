#include "IndexTracker.hpp"
#include <stdexcept>

/*
    returns the next index in the list
    otherwise raise an exception

    Returns
    -------
        int : the next index

*/
IndexTracker::ID IndexTracker::SafeNext(){

    std::lock_guard<std::mutex> lock(mtx_);

    if(curr_indx_>max_indx_)
        throw std::out_of_range("try to access an out of bound list of items");

    return ids_[curr_indx_++];
}

int IndexTracker::GetCurrentIndx() const
{
    std::lock_guard<std::mutex> lock(mtx_);
    return curr_indx_;
}

int IndexTracker::GetMaxIndx() const
{
    std::lock_guard<std::mutex> lock(mtx_);
    return max_indx_;
}