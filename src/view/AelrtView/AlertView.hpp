#ifndef ALERT_VIEW_HPP
#define ALERT_VIEW_HPP

#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/terminal.hpp>
#include <string>

inline ftxui::Component AlertView(std::string alertText, std::function<void()> onClose, std::function<void()> onConfirm)
{
    class AlertViewImpl : public ftxui::ComponentBase {
    	public:
        AlertViewImpl(std::string alertText, std::function<void()> onClose, std::function<void()> onConfirm) :
            alertText_(alertText), onClose_(onClose), onConfirm_(onConfirm)
            {

              auto buttonOptions = ftxui::ButtonOption::Simple();
              buttonOptions.transform = [](const ftxui::EntryState& s) {
                  auto content = ftxui::text(s.label);
                  if(s.focused)
                  {
                    return content | ftxui::border | ftxui::inverted;
                  }

                  return content | ftxui::border;
              };
              confirmButton_ = ftxui::Button("Confirm", onConfirm, buttonOptions);
              closeButton_ = ftxui::Button("Close", onClose, buttonOptions);
              this->Add(confirmButton_);
              this->Add(closeButton_);
              buttonsRow_ = ftxui::Container::Horizontal({confirmButton_, ftxui::Renderer([]{return ftxui::separatorEmpty() | ftxui::flex; }) ,closeButton_});
              this->Add(buttonsRow_);

            };

        ftxui::Element OnRender() override
        {
            auto dimensions = ftxui::Terminal::Size();

            return ftxui::vbox(
              ftxui::paragraphAlignCenter(alertText_),
              ftxui::separatorEmpty() | ftxui::flex_grow,
              buttonsRow_->Render()
            ) | ftxui::bgcolor(ftxui::Color::Black) | ftxui::borderRounded |
                ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, static_cast<int>(dimensions.dimy * screenPercentage_)) |
                ftxui::size(ftxui::WIDTH, ftxui::EQUAL, static_cast<int>(dimensions.dimx * screenPercentage_));
        }

        bool Focusable() const override {
            return true;
        }


        bool OnEvent(ftxui::Event event) override {
            if (event == ftxui::Event::Escape) {
              onClose_();
              return true;
            }


            return ftxui::ComponentBase::OnEvent(event);
        }
      private:
        const float screenPercentage_{0.40f};
        std::string alertText_;
        std::function<void()> onClose_;
        std::function<void()> onConfirm_;
        ftxui::Component buttonsRow_;
        ftxui::Component confirmButton_;
        ftxui::Component closeButton_;
    };


    return ftxui::Make<AlertViewImpl>(alertText, onClose, onConfirm);
}



#endif // ALERT_VIEW_HPP
