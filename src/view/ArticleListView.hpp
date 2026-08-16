#ifndef ARTICLELISTVIEW_H
#define ARTICLELISTVIEW_H
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/mouse.hpp>
#include <format>
#include "utility/utility.h"
#include "model/DataBlock.hpp"

class ArticleEntry : public ftxui::ComponentBase
{
public:
    ArticleEntry(StoryDataBlock article) : article_(article) {};

    ftxui::Element OnRender() override
    {
        auto content = ftxui::vbox({ftxui::text(article_.title_) | ftxui::notflex,
                                    ftxui::separator(),
                                    ftxui::hbox({ftxui::text(article_.author_) | ftxui::notflex,
                                                 ftxui::text(" - ") | ftxui::notflex,
                                                 ftxui::text(utils::GetDateFromUnix(article_.time_)) | ftxui::notflex,
                                                 ftxui::text(" - ") | ftxui::notflex,
                                                 ftxui::text(std::format("Score: {}", article_.score_)) | ftxui::notflex,
                                                 ftxui::text(" - ") | ftxui::notflex,
                                                 ftxui::text(std::format("Comments({})", article_.descendants_)) | ftxui::notflex}),
                                    
                                }) | ftxui::notflex;

        // Add visual cues when highlighted by keyboard navigation
        if (Focused())
        {
            content = content | inverted | focus;
        }
        return content | reflect(box_);
    }

    bool OnEvent(ftxui::Event event) override
    {
        
        if(event.is_mouse())
        {
            if(event.mouse().button == ftxui::Mouse::Left && event.mouse().motion == ftxui::Mouse::Pressed)
            {
                if(box_.Contain(event.mouse().x, event.mouse().y))
                {
                    TakeFocus();
                    return true;
                }
            }
        }

        return ftxui::ComponentBase::OnEvent(event);
    }

    bool  Focusable() const override
    {
        return true;
    }

private:
    Box box_;
    StoryDataBlock article_;
};

ftxui::Component ArticleList(std::shared_ptr<std::vector<StoryDataBlock>> articles)
{
      std::vector<ftxui::Component> entries;

    for (auto& article : *articles) {
        entries.push_back(
            ftxui::Make<ArticleEntry>(article)
        );
    }

    return ftxui::Container::Vertical(std::move(entries));
    
}

#endif