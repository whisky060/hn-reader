#ifndef MANUVIEW_HPP
#define MANUVIEW_HPP

#include "theme/themed.hpp"
#include "theme/themes.hpp"
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/terminal.hpp>
#include <functional>
#include <vector>


inline ftxui::Component MenuView(std::function<void()> onClose)
{
  class MenuViewImpl : public ftxui::ComponentBase
  {
    public:
      MenuViewImpl(std::function<void()> onClose){
        onClose_ = onClose;
        selected = 0;
        entries = AvailableThemes();
        auto menuOptions = themed::MenuOptions(entries,&selected);
        menu_ = ftxui::Menu(entries,&selected,menuOptions);
      };
      ~MenuViewImpl() = default;

      ftxui::Element OnRender() override
      {

        auto dimensions = ftxui::Terminal::Size();

        return ftxui::vbox(
          menu_->Render()
        ) | themed::Border(ftxui::BorderStyle::ROUNDED) |
            ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, static_cast<int>(dimensions.dimy * screenPercentage_)) |
            ftxui::size(ftxui::WIDTH, ftxui::EQUAL, static_cast<int>(dimensions.dimx * screenPercentage_));
      }

      bool OnEvent(ftxui::Event event) override
      {
        if (event == ftxui::Event::Escape) {
          // TODO save before closing
          onClose_();
          return true;
        }

        return ftxui::ComponentBase::OnEvent(event);

      }

      bool Focusable() const override
      {
        return true;
      }

    private:
      std::vector<std::string> entries;
      int selected;
      ftxui::Component menu_;
      std::function<void()> onClose_;
      const float screenPercentage_{0.40f};

  };


  return ftxui::Make<MenuViewImpl>(onClose);
}



#endif
