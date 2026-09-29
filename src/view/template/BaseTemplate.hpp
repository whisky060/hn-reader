#ifndef BASETEMPLATE_H
#define BASETEMPLATE_H


#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <memory>
#include <stack>
#include "utility/utility.h"
#include "view/template/Template.hpp"

inline std::shared_ptr<AbstractTemplate> BaseTemplate(ftxui::Component main_view)
{

    class ImplBaseTemplate : public AbstractTemplate
    {
    public:
        ImplBaseTemplate(ftxui::Component main_view) : AbstractTemplate()
        {

            mainContainer = ftxui::Container::Stacked({});
            mainContainer->Add(ftxui::Renderer([&]
                                   {
                                  		return ftxui::vbox({
                                       	viewStack_.top()->Render() | ftxui::flex ,
                                        ftxui::filler() | ftxui::flex,
                                        ftxui::separatorCharacter("-") | ftxui::notflex,
                                        bottom_bar_->Render() | ftxui::notflex
                                   		});
                                   }
            				));

            viewStack_.push(main_view);
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
            this->Add(viewStack_.top());
            this->Add(bottom_bar_);

        }

        ftxui::Element OnRender() override
        {
            time_ = utils::GetCurrentDate();
            return mainContainer->Render();
        }

        void ChangeView(ftxui::Component new_view) override
        {
        	auto top = viewStack_.top();
        	if (top)
        		top->Detach();
            viewStack_.push(new_view);
            this->Add(viewStack_.top());
            viewStack_.top()->TakeFocus();
        }

        void PopView() override
        {
            if(viewStack_.empty())
                return;
            auto oldView = viewStack_.top();
            viewStack_.pop();
            oldView->Detach();

            if (viewStack_.empty())
            {
            	return;
            }
           	auto top = viewStack_.top();
           	this->Add(top);
           	top->TakeFocus();
        }

        void OpenCtrlP()
        {
        }

        void PushAlert(ftxui::Component alert) override
        {
            mainContainer->Add(alert | ftxui::center);
            alert->TakeFocus();
        }

        void CloseAlert() override
        {
        	auto alert = mainContainer->ActiveChild();
        	if (alert)
        		alert->Detach();
        }

        bool OnEvent(ftxui::Event event) override
        {
            if (event == ftxui::Event::CtrlP)
            {
                OpenCtrlP();
                return true;
            }

            if (event == ftxui::Event::Escape)
            {
                CloseAlert();
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
        std::stack<ftxui::Component> viewStack_;
        ftxui::Component bottom_bar_;
        ftxui::Component mainContainer;
    };

    return std::shared_ptr<AbstractTemplate>(new ImplBaseTemplate(main_view));
}



#endif
