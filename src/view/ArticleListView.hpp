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
#include "view/ArticleEntry.hpp"

ftxui::Component ArticleList(std::shared_ptr<std::vector<StoryDataBlock>> articles)
{

    class ImplArticleList : public ftxui::ComponentBase
    {
    public:
        ImplArticleList(std::shared_ptr<std::vector<StoryDataBlock>> articles) : ftxui::ComponentBase()
        {
            articles_ = articles;
            for (auto &article : *articles_)
            {
                entries_.push_back(ftxui::Make<ArticleEntry>(article));
            }
            list_ = ftxui::Container::Vertical(std::move(entries_));

            auto button_options = ftxui::ButtonOption::Simple();
            button_options.transform = [](const ftxui::EntryState &state)
            {
                auto content = ftxui::text(state.label);
                if (state.focused)
                {
                    return content | ftxui::inverted | ftxui::center | ftxui::border;
                }
                return content | ftxui::center | ftxui::border;
            };

            auto load_more_button = ftxui::Button("Load More", []()
                                                  {
                                                      // Implement the logic to load more articles here
                                                  },
                                                  button_options);
            load_more_button_ = load_more_button;
            this->Add(list_);
            this->Add(load_more_button_);
        }

        ftxui::Element OnRender() override
        {   
            return ftxui::vbox({
                list_->Render(),
                load_more_button_->Render()
            });
        }

    private:
        ftxui::Component load_more_button_;
        std::shared_ptr<std::vector<StoryDataBlock>> articles_;
        std::vector<ftxui::Component> entries_;
        ftxui::Component list_;
    };
    std::vector<ftxui::Component> entries;

    return ftxui::Make<ImplArticleList>(articles);
}

#endif