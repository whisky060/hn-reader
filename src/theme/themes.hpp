#ifndef THEMES_HPP
#define THEMES_HPP


#include <nlohmann/json.hpp>
#include "utility/utility.h"
#include <filesystem>
#include <fstream>
#include <ftxui/component/app.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/screen/color.hpp>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#ifdef RGB
#undef RGB
#endif

inline std::vector<std::string> AvailableThemes()
{
  std::vector<std::string> themes = {
    "hn light",
    "hn dark",
    "dracula",
    "solarized dark",
    "solarized light",
    "ashen",
    "rose pine",
    "rose pine dawn"
  };
  const auto builtinCount = themes.size();

  try
  {
    const auto themeDir = utils::GetThemeDir();
    for(const auto& file : std::filesystem::directory_iterator(themeDir))
    {
      if(!file.is_regular_file() || file.path().extension() != ".json")
        continue;

      auto themeName = file.path().stem().string();
      if(themeName.empty())
        continue;

      themes.push_back(std::move(themeName));
    }
  }
  catch(const std::filesystem::filesystem_error&)
  {
  }


  return themes;
}

struct Pallet {
  ftxui::Color bg;
  ftxui::Color fg;
  ftxui::Color primary;
  ftxui::Color secondary;
  ftxui::Color border;
  ftxui::Color focus_bg;
  ftxui::Color focus_fg;

  static Pallet PalletFactory(std::string themeName)
  {

    if( themeName == "hn light")
    {

      static Pallet hn_light{
          .bg        = ftxui::Color::RGB(247, 243, 236),    // #F7F3EC
          .fg        = ftxui::Color::RGB(52, 47, 41),       // #342F29
          .primary   = ftxui::Color::RGB(180, 83, 9),       // #B45309
          .secondary = ftxui::Color::RGB(217, 119, 38),     // #D97726
          .border    = ftxui::Color::RGB(220, 211, 199),    // #DCD3C7
          .focus_bg  = ftxui::Color::RGB(239, 232, 222),    // #EFE8DE
          .focus_fg  = ftxui::Color::RGB(143, 61, 8),       // #8F3D08
      };

      return hn_light;
    }

    if(themeName == "hn dark")
    {
      static Pallet hn_dark{
          .bg        = ftxui::Color::RGB(32, 32, 32),       // #202020
          .fg        = ftxui::Color::RGB(231, 226, 218),    // #E7E2DA
          .primary   = ftxui::Color::RGB(255, 159, 67),     // #FF9F43
          .secondary = ftxui::Color::RGB(212, 119, 50),     // #D47732
          .border    = ftxui::Color::RGB(67, 64, 60),       // #43403C
          .focus_bg  = ftxui::Color::RGB(53, 48, 42),       // #35302A
          .focus_fg  = ftxui::Color::RGB(255, 183, 112),    // #FFB770
      };
      return hn_dark;
    }

    if(themeName == "dracula")
    {
      static Pallet dracula_theme{
          .bg       = ftxui::Color::RGB(40, 42, 54),
          .fg       = ftxui::Color::RGB(248, 248, 242),
          .primary  = ftxui::Color::RGB(189, 147, 249),
          .secondary = ftxui::Color::RGB(139, 233, 253),
          .border   = ftxui::Color::RGB(98, 114, 164),
          .focus_bg = ftxui::Color::RGB(68, 71, 90),
          .focus_fg = ftxui::Color::RGB(248, 248, 242),
      };

      return dracula_theme;
    }

    if(themeName == "solarized dark")
    {
      static Pallet solarized_dark{
          .bg        = ftxui::Color::RGB(0, 43, 54),
          .fg        = ftxui::Color::RGB(131, 148, 150),
          .primary   = ftxui::Color::RGB(38, 139, 210),
          .secondary = ftxui::Color::RGB(42, 161, 152),
          .border    = ftxui::Color::RGB(88, 110, 117),
          .focus_bg  = ftxui::Color::RGB(7, 54, 66),
          .focus_fg  = ftxui::Color::RGB(147, 161, 161),
      };
      return solarized_dark;
    }

    if(themeName == "solarized light")
    {
      static Pallet solarized_light{
          .bg        = ftxui::Color::RGB(253, 246, 227),
          .fg        = ftxui::Color::RGB(101, 123, 131),
          .primary   = ftxui::Color::RGB(38, 139, 210),
          .secondary = ftxui::Color::RGB(42, 161, 152),
          .border    = ftxui::Color::RGB(147, 161, 161),
          .focus_bg  = ftxui::Color::RGB(238, 232, 213),
          .focus_fg  = ftxui::Color::RGB(88, 110, 117),
      };
      return  solarized_light;;
    }

    if(themeName == "ashen")
    {
      static Pallet ashen{
          .bg        = ftxui::Color::RGB(28, 28, 36),
          .fg        = ftxui::Color::RGB(188, 188, 188),
          .primary   = ftxui::Color::RGB(212, 119, 102),
          .secondary = ftxui::Color::RGB(212, 154, 112),
          .border    = ftxui::Color::RGB(72, 70, 78),
          .focus_bg  = ftxui::Color::RGB(52, 51, 59),
          .focus_fg  = ftxui::Color::RGB(232, 218, 204),
      };
      return ashen;
    }

    if(themeName == "rose pine")
    {
      static Pallet rose_pine{
          .bg        = ftxui::Color::RGB(25, 23, 36),     // #191724
          .fg        = ftxui::Color::RGB(224, 222, 244),  // #e0def4
          .primary   = ftxui::Color::RGB(196, 167, 231),  // #c4a7e7
          .secondary = ftxui::Color::RGB(156, 207, 216),  // #9ccfd8
          .border    = ftxui::Color::RGB(64, 61, 82),     // #403d52
          .focus_bg  = ftxui::Color::RGB(38, 35, 58),    // #26233a
          .focus_fg  = ftxui::Color::RGB(224, 222, 244),  // #e0def4
      };
      return rose_pine;
    }

    if(themeName == "rose pine dawn")
    {
      static Pallet rose_pine_dawn{
          .bg        = ftxui::Color::RGB(250, 244, 237),  // #faf4ed
          .fg        = ftxui::Color::RGB(87, 82, 121),    // #575279
          .primary   = ftxui::Color::RGB(144, 122, 169),  // #907aa9
          .secondary = ftxui::Color::RGB(86, 148, 159),   // #56949f
          .border    = ftxui::Color::RGB(223, 218, 217),  // #dfdad9
          .focus_bg  = ftxui::Color::RGB(242, 233, 225),  // #f2e9e1
          .focus_fg  = ftxui::Color::RGB(87, 82, 121),    // #575279
      };
      return rose_pine_dawn;
    }

    return CreateFromFile(themeName);
  }
  static Pallet CreateFromFile(std::string fileName)
  {
    if(fileName.size() > 5 && fileName.ends_with(".json"))
      fileName.erase(fileName.size() - 5);

    auto themeFile = utils::GetThemeDir() / (fileName + ".json");
    std::ifstream file(themeFile);
    if(!file.is_open())
      throw std::runtime_error("Unable to open theme file: " + themeFile.string());

    nlohmann::json j;
    try
    {
      file >> j;
    }
    catch(const std::exception& e)
    {
      throw std::runtime_error("Unable to parse theme file '" + themeFile.string() +
                               "': " + e.what());
    }

    if(!j.is_object())
      throw std::runtime_error("Theme file '" + themeFile.string() +
                               "' must contain a JSON object");

    auto readColor = [&](const char* key) -> ftxui::Color
    {
      auto value = j.find(key);
      if(value == j.end())
        return ftxui::Color::RGB(255, 255, 255);

      if(!value->is_string())
        throw std::runtime_error("Theme file '" + themeFile.string() +
                                 "': field '" + key + "' must be a string");

      try
      {
        return utils::HexToRGB(value->get<std::string>());
      }
      catch(const std::exception& e)
      {
        throw std::runtime_error("Theme file '" + themeFile.string() +
                                 "': invalid color for '" + key + "': " + e.what());
      }
    };

    return Pallet{
      .bg = readColor("background"),
      .fg = readColor("foreground"),
      .primary = readColor("primary"),
      .secondary = readColor("secondary"),
      .border = readColor("border"),
      .focus_bg = readColor("focus_background"),
      .focus_fg = readColor("focus_foreground")
    };
  }
};


class Theme
{
  public:
    Theme()
    {
      try
      {
        auto config = utils::GetConfigFile();
        std::ifstream file(config);
        if(!file.is_open())
          throw std::runtime_error("Unable to open configuration file: " +
                                   config.string());

        nlohmann::json j;
        file >> j;

        auto themeName = j.value("theme", "hn dark");
        pallet_ = Pallet::PalletFactory(themeName);
        activeTheme_ = themeName;
      }
      catch(const std::exception& e)
      {
        pallet_ = Pallet::PalletFactory("hn dark");
        activeTheme_ = "hn dark";
        std::cerr << "Failed to load theme: " << e.what() << '\n';
      }
    };
    ~Theme(){
      SaveTheme();
    };
    static Theme& GetInstance()
    {
      static Theme theme;
      return theme;
    }

    void SaveTheme()
    {
      try{
        std::string activeTheme;
        {
          std::lock_guard<std::mutex> lk(mutex_);
          activeTheme = activeTheme_;
        }
        auto configFile = utils::GetConfigFile();
        nlohmann::json content;
        content["theme"] = activeTheme;
        std::ofstream file(configFile);
        if(file.is_open())
          file << content;
      }
      catch(...)
      {

      }
    }

    Pallet GetPallet() const
    {
      std::lock_guard<std::mutex> lk(mutex_);
      return pallet_;
    }

    void SetPallet(const std::string& palletName)
    {
      std::thread([this,palletName]{
        try
        {
          auto pallet = Pallet::PalletFactory(palletName);
          {
            std::lock_guard<std::mutex> lk(mutex_);
            pallet_ = pallet;
            activeTheme_ = palletName;
          }
          SaveTheme();
          if(auto* app = ftxui::App::Active())
            app->PostEvent(ftxui::Event::Custom);
        }
        catch(const std::exception& e)
        {

        }
      }).detach();

    }

    std::string GetActiveTheme() const
    {
      std::lock_guard<std::mutex> lk(mutex_);
      return activeTheme_;
    }

  private:
    Pallet pallet_;
    std::string activeTheme_ = "hn dark";
    mutable std::mutex mutex_;

};




#endif
