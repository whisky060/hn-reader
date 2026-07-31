#include <ftxui/component/component.hpp> // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp>  // for Component, ComponentBase

using namespace ftxui;

Component NewsListView() {
    class NewsList : public ComponentBase
    {
        


    };

    return Make<NewsList>();
}