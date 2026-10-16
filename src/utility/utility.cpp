#include <algorithm>
#include <chrono>
#include <ctime>
#include "utility/utility.h"

namespace utils {

    namespace
    {
        std::string ShellQuote(const std::string& value)
        {
            std::string quoted = "'";
            for (char c : value)
            {
                if (c == '\'')
                    quoted += "'\\''";
                else
                    quoted += c;
            }
            quoted += "'";
            return quoted;
        }
    }

    void Trim(std::string &s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
              }));
      s.erase(std::find_if(s.rbegin(), s.rend(),
                           [](unsigned char ch) { return !std::isspace(ch); })
                  .base(),
              s.end());
    }


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
      if (url.empty())
          return;

      #if defined(_WIN32) || defined(_WIN64)
        ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
      #elif defined(__APPLE__)
        std::string command = "open " + ShellQuote(url);
        std::system(command.c_str());
      #elif defined(__linux__)
        std::string command = "xdg-open " + ShellQuote(url) + " &";
        std::system(command.c_str());
      #else
        std::cerr << "Unsupported operating system." << std::endl;
      #endif
    }

}
