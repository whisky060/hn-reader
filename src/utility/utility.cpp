#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <functional>
#include <string>
#include "utility/utility.h"
#include <nlohmann/json.hpp>

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

    ftxui::Color HexToRGB(std::string color)
    {
      unsigned int hexValue = 0;
      std::stringstream ss;
      ss << std::hex << color;
      ss >> hexValue;

      auto r = (hexValue >> 16) & 0xFF; // Shift right 16 bits to get Red
      auto g = (hexValue >> 8) & 0xFF;  // Shift right 8 bits to get Green
      auto b = hexValue & 0xFF;         // Mask the last 8 bits to get Blue

      return ftxui::Color(r,g,b);
    }
    std::filesystem::path GetConfigFile()
    {
      std::function<std::string(const char*)> env = [](const char* envVAR){
        const char* v = std::getenv(envVAR);
        return v ? v : "";
      };

      std::string p;
      std::filesystem::path pth;



      #if defined(_WIN32) || defined(_WIN64)

        p = env("APPDATA");
        if(p.empty())
        {
          p = env("USERPROFILE");
          pth = std::filesystem::path(p) / "AppData" / "Roaming";
        }
        else
        {
          pth = std::filesystem::path(p);
        }
      #else
        p = std::getenv("XDG_CONFIG_HOME");
        if(p.empty())
        {
          p = env("HOME");
          pth = std::filesystem::path(p) / ".config";
        }
        else
        {
          pth = std::filesystem::path(p);
        }

      #endif

      pth = pth / appName;
      if(!std::filesystem::exists(pth))
        std::filesystem::create_directory(pth);

      auto configFile = pth / configName;
      if(!std::filesystem::exists(configFile))
      {
        nlohmann::json content;
        content["theme"] = "hn dark";
        std::ofstream file(configFile);
        if(file.is_open())
          file << content;

      }
      return configFile;
    }


    std::filesystem::path GetThemeDir()
    {
      std::function<std::string(const char*)> env = [](const char* envVAR){
        const char* v = std::getenv(envVAR);
        return v ? v : "";
      };

      std::string p;
      std::filesystem::path pth;



      #if defined(_WIN32) || defined(_WIN64)

        p = env("APPDATA");
        if(p.empty())
        {
          p = env("USERPROFILE");
          pth = std::filesystem::path(p) / "AppData" / "Roaming";
        }
        else
        {
          pth = std::filesystem::path(p);
        }
      #else
        p = std::getenv("XDG_CONFIG_HOME");
        if(p.empty())
        {
          p = env("HOME");
          pth = std::filesystem::path(p) / ".config";
        }
        else
        {
          pth = std::filesystem::path(p);
        }

      #endif

      pth = pth / appName;
      if(!std::filesystem::exists(pth))
        std::filesystem::create_directory(pth);

      auto themesDir = pth / themeDirName;
      if(!std::filesystem::exists(pth))
        std::filesystem::create_directory(pth);

      return themesDir;

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
