#ifndef WRAPCOLLAPSIBLE_HPP
#define WRAPCOLLAPSIBLE_HPP

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <utility>

// Copy of ftxui::Collapsible (collapsible.cpp) with two changes:
//  1) the label uses paragraph() so it wraps instead of overflowing
//  2) the child is indented with empty separator columns
inline ftxui::Component WrapCollapsible(ftxui::ConstStringRef label,
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
        WrapCollapsibleImpl(ftxui::ConstStringRef label,
                            ftxui::Component child,
                            bool *show)
        {
            ftxui::CheckboxOption opt;
            opt.transform = [](ftxui::EntryState s) {
                auto prefix = ftxui::text(s.state ? "▼ " : "▶ ") | ftxui::notflex;
                auto t = ftxui::hflow(ftxui::paragraph(s.label)) | ftxui::flex;   // was: text(s.label)
                if (s.active)  t |= ftxui::bold;
                if (s.focused) t |= ftxui::inverted;
                return ftxui::hbox({prefix, t});
            };
            this->Add(ftxui::Container::Vertical({
                ftxui::Checkbox(label, show, opt),
                ftxui::Maybe(child, show),
            }));
        }
    };
    return ftxui::Make<WrapCollapsibleImpl>(label, std::move(indented), show);
}

#endif // WRAPCOLLAPSIBLE_HPP
