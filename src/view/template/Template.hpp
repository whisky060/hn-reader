#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <thread>
#include "utility/utility.h"



/*
    returns a template made of: a bottom bar an at the center a view
    that needs to be rendered as the main view
*/

// implements the template in the bottom bar we have a clock (right) and a list of commands left

class AbstractTemplate : public ftxui::ComponentBase
{

public:
    AbstractTemplate(ftxui::Component main_view) : ftxui::ComponentBase(), main_view_(main_view) {}
    virtual ~AbstractTemplate() = default;
    virtual void ChangeView(ftxui::Component new_view) = 0;

private:
    ftxui::Component main_view_;
};

#endif