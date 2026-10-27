#ifndef BASETEMPLATE_H
#define BASETEMPLATE_H


#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <memory>
#include <stack>
#include <vector>
#include "theme/themed.hpp"
#include "utility/utility.h"
#include "view/template/Template.hpp"
#include "view/BaseView.hpp"

inline std::shared_ptr<AbstractTemplate> BaseTemplate(std::shared_ptr<BaseView> main_view)
{

    class ImplBaseTemplate : public AbstractTemplate
    {
    public:
        ImplBaseTemplate(std::shared_ptr<BaseView> main_view) : AbstractTemplate()
        {

            alertOpen_ = false;
            alertContainer_ = ftxui::Container::Vertical({});
            viewStack_.push(main_view);

            time_ = TrimNewline(utils::GetCurrentDate());
            // Bottom bar is re-rendered every frame from ActiveView()->GetShortcuts(),
            // so any view pushed via ChangeView automatically updates the bar.
            // Rendered as plain text chips (non-focusable) to avoid stealing
            // keyboard focus from the active view.
            bottom_bar_ = ftxui::Renderer([this]
                                   {
                                        ftxui::Elements chips;
                                        auto active = ActiveView();
                                        if (active)
                                        {
                                            bool first = true;
                                            for (const auto& sc : active->GetShortcuts())
                                            {
                                                if (!first)
                                                    chips.push_back(themed::Text("  "));
                                                  chips.push_back(ftxui::hbox({
                                                    themed::Text(sc.key+" : "),
                                                    themed::Text(" " + sc.label),
                                                }));
                                                first = false;
                                            }
                                        }
                                        auto left = ftxui::hbox(std::move(chips));
                                        return ftxui::hbox({std::move(left) | ftxui::notflex,
                                                        ftxui::filler(),
                                                        themed::Text(time_) | ftxui::notflex
                                      }) | ftxui::notflex;
                                   });
            mainView_ = ftxui::Renderer([&]
                                   {
                                  		return ftxui::vbox({
                                       	ActiveRender() | ftxui::flex ,
                                        ftxui::filler() | ftxui::flex,
                                        themed::SeparatorCharacter("-") | ftxui::notflex,
                                        bottom_bar_->Render() | ftxui::notflex | themed::Base()
                                   		});
                                   }
            				);
            mainContainer = mainView_ | ftxui::Modal(alertContainer_, &alertOpen_);

            this->Add(mainContainer);
            this->Add(viewStack_.top());
            this->Add(bottom_bar_);



        }

        ftxui::Element OnRender() override
        {
            time_ = TrimNewline(utils::GetCurrentDate());
            return mainContainer->Render() | themed::Base();
        }

        void ChangeView(std::shared_ptr<BaseView> new_view) override
        {
        	auto top = ActiveView();
        	if (top)
        		top->Detach();
            viewStack_.push(new_view);
            this->Add(viewStack_.top());
            viewStack_.top()->TakeFocus();
        }

        std::shared_ptr<BaseView> ActiveView() override
        {
            if (viewStack_.empty())
                return nullptr;
            return viewStack_.top();
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


        void PushAlert(ftxui::Component alert) override
        {
            alertContainer_->DetachAllChildren();
            alertOpen_ = true;
            alertContainer_->Add(ftxui::Renderer(alert, [alert] {
                return alert->Render() | ftxui::center | themed::Base();
            }));
            alertContainer_->TakeFocus();
        }

        void CloseAlert() override
        {
            alertOpen_ = false;
            alertContainer_->DetachAllChildren();
        }

        bool OnEvent(ftxui::Event event) override
        {

            if (event == ftxui::Event::Escape && mainContainer->ChildCount() > 1)
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
        static std::string TrimNewline(std::string s)
        {
            while (!s.empty() && (s.back() == '\n' || s.back() == '\r'))
                s.pop_back();
            return s;
        }

        ftxui::Element ActiveRender()
        {
            auto active = ActiveView();
            if (!active)
                return ftxui::text("no view") | ftxui::center;
            return active->Render();
        }

        std::string time_;
        std::stack<std::shared_ptr<BaseView>> viewStack_;
        ftxui::Component bottom_bar_;
        ftxui::Component mainContainer;
        ftxui::Component alertContainer_;
        ftxui::Component mainView_;
        bool alertOpen_ = false;
    };

    return std::shared_ptr<AbstractTemplate>(new ImplBaseTemplate(main_view));
}



#endif
