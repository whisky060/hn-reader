#ifndef BASETEMPLATE_H
#define BASETEMPLATE_H

#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <thread>
#include "utility/utility.h"
#include "view/template/Template.hpp"

inline ftxui::Component BaseTemplate(ftxui::Component main_view)
{

    class ImplBaseTemplate : public AbstractTemplate
    {
    public:
        ImplBaseTemplate(ftxui::Component main_view) : AbstractTemplate(main_view)
        {
            main_view_ = main_view;
            time_ = utils::GetCurrentDate();
            auto button_options = ftxui::ButtonOption::Simple();
            button_options.transform = [](const ftxui::EntryState& state)
            {   
                auto content = ftxui::text(state.label);
                if (state.focused)
                {
                    return content;
                }
                return content | ftxui::bgcolor(ftxui::Color::Black);
            };

            ftxui::Component button = ftxui::Button("ctrl+p", [this]()
                                      { this->OpenCtrlP(); }, button_options);
            bottom_bar_ = ftxui::Renderer([button, this]
                                   { return ftxui::hbox({button->Render() | ftxui::notflex,
                                                        ftxui::filler(),
                                                        ftxui::text(time_) | ftxui::notflex
                                                    }) | ftxui::notflex;
                                   });
            this->Add(main_view_);
            this->Add(bottom_bar_);
            
        }

        ftxui::Element OnRender() override
        {   
            time_ = utils::GetCurrentDate();      
            return ftxui::vbox({
                main_view_->Render()  | ftxui::vscroll_indicator | ftxui::frame | ftxui::flex ,
                ftxui::filler() | ftxui::flex,
                ftxui::separatorCharacter("-") | ftxui::notflex,
                bottom_bar_->Render() | ftxui::notflex 
            });
        }

        void ChangeView(ftxui::Component new_view) override
        {
            main_view_ = new_view;
        }

        void OpenCtrlP()
        {
        }

        bool OnEvent(ftxui::Event event) override
        {
            if (event == ftxui::Event::CtrlP)
            {
                OpenCtrlP();
                return true;
            }

            
             if(ftxui::ComponentBase::OnEvent(event))
             {
                 return true;
             }
            

            return false;
        }

        

    private:
        std::string time_;
        ftxui::Component main_view_;
        ftxui::Component bottom_bar_;
    };

    return ftxui::Make<ImplBaseTemplate>(main_view);
}



#endif