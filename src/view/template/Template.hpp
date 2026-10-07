#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <memory>

#include "view/BaseView.hpp"

/*
    returns a template made of: a bottom bar an at the center a view
    that needs to be rendered as the main view
*/

// implements the template in the bottom bar we have a clock (right) and a list of commands left

class AbstractTemplate : public ftxui::ComponentBase
{

    public:
        AbstractTemplate() = default;
        virtual ~AbstractTemplate() = default;
        virtual void ChangeView(std::shared_ptr<BaseView> new_view) = 0;
        virtual void PushAlert(ftxui::Component alert) = 0;
        virtual void CloseAlert() = 0;
        virtual void PopView() = 0;
        // Exposes the currently visible view so chrome (title / bottom bar)
        // can be derived from it. Returns nullptr when stack is empty.
        virtual std::shared_ptr<BaseView> ActiveView() = 0;


    private:


};

#endif
