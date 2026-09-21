#ifndef ARTICLELISTVIEW_H
#define ARTICLELISTVIEW_H
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/mouse.hpp>
#include <string>
#include "utility/utility.h"
#include "model/DataBlock.hpp"
#include "view/ArticleList/ArticleEntry.hpp"
#include "view/Spinner/spinner.hpp"

inline ftxui::Component ArticleList(
							std::function<std::shared_ptr<const std::vector<StoryDataBlock>>()> get_articles,
                            std::function<void(std::string)> on_select,
                            std::function<void()> on_load,
                            std::function<bool()> is_loading
						)

{

    class ImplArticleList : public ftxui::ComponentBase
    {
    public:
        ImplArticleList(std::function<std::shared_ptr<const std::vector<StoryDataBlock>>()> get_articles,
                        std::function<void()> on_load,
                        std::function<void(std::string)> on_select,
                        std::function<bool()> is_loading) : ftxui::ComponentBase()
        {
            get_articles_ = get_articles;
            articles_ = get_articles_();
            on_load_ = on_load;
            on_select_ = on_select;
            is_loading_ = is_loading;
            std::string first_pattern[] = {

                "█   ▒ ▒   ▒       ▒▒▒▒  ▒▒▒▒▒  ▒▒▒  ▒▒▒▒  ▒▒▒▒▒ ▒▒▒▒\n"
                "█   ▒ ▒▒  ▒       ▒   ▒ ▒     ▒   ▒ ▒   ▒ ▒     ▒   ▒\n"
                "█▒▒▒▒ ▒ ▒ ▒       ▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒ ▒   ▒ ▒▒▒▒  ▒▒▒▒\n"
                "█   ▒ ▒  ▒▒       ▒ ▒   ▒     ▒   ▒ ▒   ▒ ▒     ▒ ▒\n"
                "█   ▒ ▒   ▒       ▒  ▒  ▒▒▒▒▒ ▒   ▒ ▒▒▒▒  ▒▒▒▒▒ ▒  ▒",

                "▒  ░▓█▒   ▒       ▒▒▒▒  ▒▒▒▒▒  ▒▒▒  ▒▒▒▒  ▒▒▒▒▒ ▒▒▒▒\n"
                "▒  ░▓█▒▒  ▒       ▒   ▒ ▒     ▒   ▒ ▒   ▒ ▒     ▒   ▒\n"
                "▒▒▒░▓█▒ ▒ ▒       ▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒ ▒   ▒ ▒▒▒▒  ▒▒▒▒\n"
                "▒  ░▓█▒  ▒▒       ▒ ▒   ▒     ▒   ▒ ▒   ▒ ▒     ▒ ▒\n"
                "▒  ░▓█▒   ▒       ▒  ▒  ▒▒▒▒▒ ▒   ▒ ▒▒▒▒  ▒▒▒▒▒ ▒  ▒",

                "▒   ▒ ▒ ░▓█       ▒▒▒▒  ▒▒▒▒▒  ▒▒▒  ▒▒▒▒  ▒▒▒▒▒ ▒▒▒▒\n"
                "▒   ▒ ▒▒░▓█       ▒   ▒ ▒     ▒   ▒ ▒   ▒ ▒     ▒   ▒\n"
                "▒▒▒▒▒ ▒ ░▓█       ▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒ ▒   ▒ ▒▒▒▒  ▒▒▒▒\n"
                "▒   ▒ ▒ ░▓█       ▒ ▒   ▒     ▒   ▒ ▒   ▒ ▒     ▒ ▒\n"
                "▒   ▒ ▒ ░▓█       ▒  ▒  ▒▒▒▒▒ ▒   ▒ ▒▒▒▒  ▒▒▒▒▒ ▒  ▒",

                "▒   ▒ ▒   ▒  ░▓█  ▒▒▒▒  ▒▒▒▒▒  ▒▒▒  ▒▒▒▒  ▒▒▒▒▒ ▒▒▒▒\n"
                "▒   ▒ ▒▒  ▒  ░▓█  ▒   ▒ ▒     ▒   ▒ ▒   ▒ ▒     ▒   ▒\n"
                "▒▒▒▒▒ ▒ ▒ ▒  ░▓█  ▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒ ▒   ▒ ▒▒▒▒  ▒▒▒▒\n"
                "▒   ▒ ▒  ▒▒  ░▓█  ▒ ▒   ▒     ▒   ▒ ▒   ▒ ▒     ▒ ▒\n"
                "▒   ▒ ▒   ▒  ░▓█  ▒  ▒  ▒▒▒▒▒ ▒   ▒ ▒▒▒▒  ▒▒▒▒▒ ▒  ▒",

                "▒   ▒ ▒   ▒       ░▓█▒  ▒▒▒▒▒  ▒▒▒  ▒▒▒▒  ▒▒▒▒▒ ▒▒▒▒\n"
                "▒   ▒ ▒▒  ▒       ░▓█ ▒ ▒     ▒   ▒ ▒   ▒ ▒     ▒   ▒\n"
                "▒▒▒▒▒ ▒ ▒ ▒       ░▓█▒  ▒▒▒▒  ▒▒▒▒▒ ▒   ▒ ▒▒▒▒  ▒▒▒▒\n"
                "▒   ▒ ▒  ▒▒       ░▓█   ▒     ▒   ▒ ▒   ▒ ▒     ▒ ▒\n"
                "▒   ▒ ▒   ▒       ░▓█▒  ▒▒▒▒▒ ▒   ▒ ▒▒▒▒  ▒▒▒▒▒ ▒  ▒",

                "▒   ▒ ▒   ▒       ▒▒▒▒ ░▓█▒▒▒  ▒▒▒  ▒▒▒▒  ▒▒▒▒▒ ▒▒▒▒\n"
                "▒   ▒ ▒▒  ▒       ▒   ▒░▓█    ▒   ▒ ▒   ▒ ▒     ▒   ▒\n"
                "▒▒▒▒▒ ▒ ▒ ▒       ▒▒▒▒ ░▓█▒▒  ▒▒▒▒▒ ▒   ▒ ▒▒▒▒  ▒▒▒▒\n"
                "▒   ▒ ▒  ▒▒       ▒ ▒  ░▓█    ▒   ▒ ▒   ▒ ▒     ▒ ▒\n"
                "▒   ▒ ▒   ▒       ▒  ▒ ░▓█▒▒▒ ▒   ▒ ▒▒▒▒  ▒▒▒▒▒ ▒  ▒",

                "▒   ▒ ▒   ▒       ▒▒▒▒  ▒▒▒▒░▓█▒▒▒  ▒▒▒▒  ▒▒▒▒▒ ▒▒▒▒\n"
                "▒   ▒ ▒▒  ▒       ▒   ▒ ▒   ░▓█   ▒ ▒   ▒ ▒     ▒   ▒\n"
                "▒▒▒▒▒ ▒ ▒ ▒       ▒▒▒▒  ▒▒▒▒░▓█▒▒▒▒ ▒   ▒ ▒▒▒▒  ▒▒▒▒\n"
                "▒   ▒ ▒  ▒▒       ▒ ▒   ▒   ░▓█   ▒ ▒   ▒ ▒     ▒ ▒\n"
                "▒   ▒ ▒   ▒       ▒  ▒  ▒▒▒▒░▓█   ▒ ▒▒▒▒  ▒▒▒▒▒ ▒  ▒",

                "▒   ▒ ▒   ▒       ▒▒▒▒  ▒▒▒▒▒  ▒▒░▓█▒▒▒▒  ▒▒▒▒▒ ▒▒▒▒\n"
                "▒   ▒ ▒▒  ▒       ▒   ▒ ▒     ▒  ░▓█▒   ▒ ▒     ▒   ▒\n"
                "▒▒▒▒▒ ▒ ▒ ▒       ▒▒▒▒  ▒▒▒▒  ▒▒▒░▓█▒   ▒ ▒▒▒▒  ▒▒▒▒\n"
                "▒   ▒ ▒  ▒▒       ▒ ▒   ▒     ▒  ░▓█▒   ▒ ▒     ▒ ▒\n"
                "▒   ▒ ▒   ▒       ▒  ▒  ▒▒▒▒▒ ▒  ░▓█▒▒▒▒  ▒▒▒▒▒ ▒  ▒",

                "▒   ▒ ▒   ▒       ▒▒▒▒  ▒▒▒▒▒  ▒▒▒  ▒▒░▓█ ▒▒▒▒▒ ▒▒▒▒\n"
                "▒   ▒ ▒▒  ▒       ▒   ▒ ▒     ▒   ▒ ▒ ░▓█ ▒     ▒   ▒\n"
                "▒▒▒▒▒ ▒ ▒ ▒       ▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒ ▒ ░▓█ ▒▒▒▒  ▒▒▒▒\n"
                "▒   ▒ ▒  ▒▒       ▒ ▒   ▒     ▒   ▒ ▒ ░▓█ ▒     ▒ ▒\n"
                "▒   ▒ ▒   ▒       ▒  ▒  ▒▒▒▒▒ ▒   ▒ ▒▒░▓█ ▒▒▒▒▒ ▒  ▒",

                "▒   ▒ ▒   ▒       ▒▒▒▒  ▒▒▒▒▒  ▒▒▒  ▒▒▒▒  ▒░▓█▒ ▒▒▒▒\n"
                "▒   ▒ ▒▒  ▒       ▒   ▒ ▒     ▒   ▒ ▒   ▒ ▒░▓█  ▒   ▒\n"
                "▒▒▒▒▒ ▒ ▒ ▒       ▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒ ▒   ▒ ▒░▓█  ▒▒▒▒\n"
                "▒   ▒ ▒  ▒▒       ▒ ▒   ▒     ▒   ▒ ▒   ▒ ▒░▓█  ▒ ▒\n"
                "▒   ▒ ▒   ▒       ▒  ▒  ▒▒▒▒▒ ▒   ▒ ▒▒▒▒  ▒░▓█▒ ▒  ▒",

                "▒   ▒ ▒   ▒       ▒▒▒▒  ▒▒▒▒▒  ▒▒▒  ▒▒▒▒  ▒▒▒▒▒ ░▓█▒\n"
                "▒   ▒ ▒▒  ▒       ▒   ▒ ▒     ▒   ▒ ▒   ▒ ▒     ░▓█ ▒\n"
                "▒▒▒▒▒ ▒ ▒ ▒       ▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒ ▒   ▒ ▒▒▒▒  ░▓█▒\n"
                "▒   ▒ ▒  ▒▒       ▒ ▒   ▒     ▒   ▒ ▒   ▒ ▒     ░▓█ ▒\n"
                "▒   ▒ ▒   ▒       ▒  ▒  ▒▒▒▒▒ ▒   ▒ ▒▒▒▒  ▒▒▒▒▒ ░▓█ ▒",

            };
            first_load_spinner_ = Spinner(first_pattern, 1000ms);
            this->Add(first_load_spinner_);

            std::string pattern[] = {
                "█N READER",
                "▓N READER",
                "H█ READER",
                "H▓ READER",
                "HN█READER",
                "HN▓READER",
                "HN █EADER",
                "HN ▓EADER",
                "HN R█ADER",
                "HN R▓ADER",
                "HN RE█DER",
                "HN RE▓DER",
                "HN REA█ER",
                "HN REA▓ER",
                "HN READ█R",
                "HN READ▓R",
                "HN READE█",
                "HN READE▓",
            };

            spinner_ = Spinner(pattern, 1000ms);
            this->Add(spinner_);

            old_state = false;
            for (auto &article : *articles_)
            {
                entries_.push_back(ftxui::Make<ArticleEntry>(article, on_select_));

            }
            list_ = ftxui::Container::Vertical(entries_);

            auto button_options = ftxui::ButtonOption::Simple();
            button_options.transform = [](const ftxui::EntryState &state)
            {
                auto content = ftxui::text(state.label);
                if (state.focused)
                {
                    return content | ftxui::inverted | ftxui::center;
                }
                return content | ftxui::center;
            };

            auto load_more_button = ftxui::Button("Load More", on_load_, button_options);
            load_more_button_ = load_more_button;
            this->Add(list_);
            list_->Add(load_more_button_);
        }

        ftxui::Element OnRender() override
        {
            bool loading = is_loading_();

            if (entries_.empty() && loading)
            {
                old_state = loading;
                return first_load_spinner_->Render() | ftxui::center;
            }

            if (old_state && !loading)
            {
                Refresh();
            }

            if (loading && !old_state)
            {
                load_more_button_->Detach();
                int indx = entries_.size()-1;
                list_->ChildAt(indx)->TakeFocus();
                list_->Add(spinner_ | ftxui::center);
            }

            old_state = loading;

            return list_->Render();
        }


        bool OnEvent(ftxui::Event event) override
        {
            return ftxui::ComponentBase::OnEvent(event);
        }

        bool Focusable() const override
        {
            return true;
        }

    private:
        void Refresh()
        {

            auto next = get_articles_();
            if (next.get() == articles_.get())
                return;

            articles_ = next;
            list_->Detach();
            int indx = entries_.size();
            entries_.clear();
            for (auto &article : *articles_)
            {
                entries_.push_back(ftxui::Make<ArticleEntry>(article, on_select_));
            }
            list_ = ftxui::Container::Vertical(entries_);
            if (articles_->size() < 500)
                list_->Add(load_more_button_);

            this->Add(list_);
            list_->ChildAt(indx)->TakeFocus();
        }

        ftxui::Component load_more_button_;
        ftxui::Component list_;
        ftxui::Component first_load_spinner_;
        ftxui::Component spinner_;
        std::shared_ptr<const std::vector<StoryDataBlock>> articles_;
        std::vector<ftxui::Component> entries_;
        std::function<void()> on_load_;
        std::function<void(std::string)> on_select_;
        std::function<bool()> is_loading_;
        std::function<std::shared_ptr<const std::vector<StoryDataBlock>>()> get_articles_;
        bool old_state;
    };

    return ftxui::Make<ImplArticleList>(get_articles, on_load, on_select, is_loading);
}

#endif
