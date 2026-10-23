#ifndef THEMES_HPP
#define THEMES_HPP


#include <ftxui/screen/color.hpp>
#include <future>
#include <mutex>



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

      static Pallet personal_light{
          .bg        = ftxui::Color::RGB(247, 243, 236),    // #F7F3EC
          .fg        = ftxui::Color::RGB(52, 47, 41),       // #342F29
          .primary   = ftxui::Color::RGB(180, 83, 9),       // #B45309
          .secondary = ftxui::Color::RGB(217, 119, 38),     // #D97726
          .border    = ftxui::Color::RGB(220, 211, 199),    // #DCD3C7
          .focus_bg  = ftxui::Color::RGB(239, 232, 222),    // #EFE8DE
          .focus_fg  = ftxui::Color::RGB(143, 61, 8),       // #8F3D08
      };
    }

    if(themeName == "hn dark")
    {
      static Pallet personal_dark{
          .bg        = ftxui::Color::RGB(32, 32, 32),       // #202020
          .fg        = ftxui::Color::RGB(231, 226, 218),    // #E7E2DA
          .primary   = ftxui::Color::RGB(255, 159, 67),     // #FF9F43
          .secondary = ftxui::Color::RGB(212, 119, 50),     // #D47732
          .border    = ftxui::Color::RGB(67, 64, 60),       // #43403C
          .focus_bg  = ftxui::Color::RGB(53, 48, 42),       // #35302A
          .focus_fg  = ftxui::Color::RGB(255, 183, 112),    // #FFB770
      };
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
    }

    return Pallet();
  }
};


class Theme
{
  public:
    Theme() = default;
    ~Theme() = default;
    static Theme& GetInstance()
    {
      static Theme theme;
      return theme;
    }

    Pallet GetPallet() {return pallet_ ; }
    void SetPallet(std::string palletName)
    {
      std::lock_guard<std::mutex> lk(mutex_);
      auto pallet = Pallet::PalletFactory(palletName);
      pallet_ = pallet;
    }

  private:
    Pallet pallet_;
    std::mutex mutex_;

};




#endif
