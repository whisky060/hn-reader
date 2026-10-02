#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <iostream>
#include <cstdlib>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#endif

namespace utils {

    using date = char*;
    date GetCurrentDate();
    date GetDateFromUnix(long long timestamp);
    void OpenUrl(const std::string& url);
}


#endif
