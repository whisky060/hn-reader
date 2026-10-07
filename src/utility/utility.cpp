#include <chrono>
#include <ctime>
#include "utility/utility.h"



namespace utils {


    /*
        function that returns the current time

        RETURNS:
            the current time
    */
    date GetCurrentDate()
    {
        // Get current time point
        auto now = std::chrono::system_clock::now();

        // Convert to time_t
        std::time_t end_time = std::chrono::system_clock::to_time_t(now);

        return std::ctime(&end_time);
    }


    /*
        function that converts a Unix timestamp to a date

        PARAMETERS:
            timestamp: the Unix timestamp to convert

        RETURNS:
            the converted date

    */
    date GetDateFromUnix(long long timestamp)
    {

        std::time_t time = static_cast<std::time_t>(timestamp);
        return std::ctime(&time);
    }

    void OpenUrl(const std::string& url)
    {
      #if defined(_WIN32) || defined(_WIN64)
          ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
      #elif defined(__APPLE__)
          std::string command = "open " + url;
          std::system(command.c_str());
      #elif defined(__linux__)
        std::string command = "xdg-open " + url + " &";
          std::system(command.c_str());
      #else
          std::cerr << "Unsupported operating system." << std::endl;
      #endif
    }
}
