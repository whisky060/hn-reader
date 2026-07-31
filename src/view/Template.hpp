#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>

using namespace ftxui;

/*
    returns a template made of: a bottom bar an at the center a view
    that needs to be rendered as the main view
*/

// implements the template in the bottom bar we have a clock (right) and a list of commands left

class AbstractTemplate : public ComponentBase
{

public:
    AbstractTemplate(Component main_view) : ComponentBase(), main_view_(main_view) {}
    virtual ~AbstractTemplate() = default;
    virtual void ChangeView(Component new_view) = 0;

private:
    Component main_view_;
};

Component BaseTemplate(Component main_view)
{

    class ImplBaseTemplate : public AbstractTemplate
    {
    public:
        ImplBaseTemplate(Component main_view) : AbstractTemplate(main_view)
        {
            Component button = Button("ctrl+p", [this]()
                                      { this->OpenCtrlP(); });
            bottom_bar_ = Renderer([button]
                                   { return hbox({button->Render()}); });
        }

        Element OnRender() override
        {
            return vbox({main_view_->Render(),
                         bottom_bar_->Render()});
        }

        void ChangeView(Component new_view)
        {
            main_view_ = new_view;
        }

        void OpenCtrlP()
        {
        }

        bool OnEvent(Event event) override
        {
            if (event == Event::CtrlP)
            {
                OpenCtrlP();
            }

            return main_view_->OnEvent(event);
        }

    private:
        Component main_view_;
        Component bottom_bar_;
    };
    return Make<ImplBaseTemplate>(main_view);
}

#endif