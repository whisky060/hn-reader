#ifndef UTILITY_H
#define UTILITY_H



namespace utils {

    using date = char*;
    date GetCurrentDate();
    date GetDateFromUnix(long long timestamp);

}


#endif