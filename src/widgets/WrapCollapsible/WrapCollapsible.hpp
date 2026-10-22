#ifndef WRAPCOLLAPSIBLE_HPP
#define WRAPCOLLAPSIBLE_HPP

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/box.hpp>
#include <utility>

// Copy of ftxui::Collapsible (collapsible.cpp) with two changes:
//  1) the label uses paragraph() so it wraps instead of overflowing
//  2) the child is indented with empty separator columns
inline ftxui::Component WrapCollapsible(ftxui::Component label,
                                        ftxui::Component child,
                                        bool *show)
{
    auto indented = ftxui::Renderer(child, [child]() -> ftxui::Element {
        if (child->ChildCount() == 0)
            return child->Render();
        return ftxui::hbox({ftxui::separatorEmpty(),
                            ftxui::separatorEmpty(),
                            child->Render() | ftxui::flex});
    });

    class WrapCollapsibleImpl : public ftxui::ComponentBase {
    public:
        WrapCollapsibleImpl(ftxui::Component label,
                            ftxui::Component child,
                            bool *show)
        {
            ftxui::CheckboxOption opt;
            opt.transform = [label,this](ftxui::EntryState s) {
                auto prefix = ftxui::text(s.state ? "▼ " : "▶ ") | ftxui::notflex;
                auto t = label->Render() | ftxui::flex;
                if (s.focused) t |= ftxui::inverted;
                return ftxui::hbox({prefix, t});
            };
            this->Add(ftxui::Container::Vertical({
                ftxui::Checkbox("", show, opt),
                ftxui::Maybe(child, show),
            }));
        }
    private:
      ftxui::Box box_;
    };
    return ftxui::Make<WrapCollapsibleImpl>(label, std::move(indented), show);
}

#endif // WRAPCOLLAPSIBLE_HPP
