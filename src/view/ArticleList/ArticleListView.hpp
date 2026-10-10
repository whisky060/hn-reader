#ifndef ARTICLELISTVIEW_H
#define ARTICLELISTVIEW_H
#include <exception>
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/mouse.hpp>
#include <memory>
#include <string>
#include <vector>
#include "concurrency/FetchResult.hpp"
#include "model/app_model.hpp"
#include "utility/utility.h"
#include "model/DataBlock.hpp"
#include "view/ArticleList/ArticleEntry.hpp"
#include "view/BaseView.hpp"
#include "widgets/Spinner/spinner.hpp"

enum class ListState
{
  FirstLoad,
  Ready,
  Error,
  Loading
};

inline std::shared_ptr<BaseView> ArticleList(
                            std::function<void(std::string)> on_select,
                            std::function<void(ListCallback)> on_load
						              )

{

    class ImplArticleList : public BaseView
    {
    public:
        ImplArticleList(
                        std::function<void(ListCallback)> on_load,
                        std::function<void(std::string)> on_select
                        ) : BaseView()
        {

            on_load_ = on_load;
            on_select_ = on_select;
            state = ListState::FirstLoad;
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

            spinner_ = Spinner(pattern, 1000ms) | ftxui::center;

            list_ = ftxui::Container::Vertical({});

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

            ListCallback callback = [this](FetchResult<std::vector<StoryDataBlock>> res){
               if(res.OK())
               {
                 this->state = ListState::Ready;
                 auto value = res.GetValue();
                 this->articles_.insert(this->articles_.end(), value.begin(), value.end());
                 this->Refresh();
               }
               else
               {
                 this->state = ListState::Error;
                 std::rethrow_exception(res.GetException());
               }
            };

            auto load_more_button = ftxui::Button("Load More", [this,callback]{
              this->AddSpinner();
              this->state = ListState::Loading;
              this->on_load_(callback);
            }, button_options);

            load_more_button_ = load_more_button;
            this->Add(list_);
            on_load_(callback);
        }

        std::string GetTitle() const override { return "Top Stories"; }

        std::vector<Shortcut> GetShortcuts() const override {
            return {
                {"Enter", "open"},
                {"Up/Down", "navigate"},
            };
        }

        ftxui::Element OnRender() override
        {


            if (state == ListState::FirstLoad)
            {
                return first_load_spinner_->Render() | ftxui::center;
            }

            return list_->Render() | ftxui::vscroll_indicator | ftxui::yframe;
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

            int indx = list_->ChildCount();

            for (int i = indx; i < articles_.size(); i++)
            {
                auto entry = ftxui::Make<ArticleEntry>(articles_.at(i), on_select_);
                list_->Add(entry);
            }

            spinner_->Detach();
            if (articles_.size() < 500)
                list_->Add(load_more_button_);
        }

        void AddSpinner()
        {
          load_more_button_->Detach();
          auto index = list_->ChildCount() - 1;
          list_->Add(spinner_);
          list_->ChildAt(index)->TakeFocus();
        }

        ftxui::Component load_more_button_;
        ftxui::Component list_;
        ftxui::Component first_load_spinner_;
        ftxui::Component spinner_;
        std::vector<StoryDataBlock> articles_;
        std::vector<ftxui::Component> entries_;
        std::function<void(ListCallback)> on_load_;
        std::function<void(std::string)> on_select_;
        ListState state;
    };

    return std::static_pointer_cast<BaseView>(ftxui::Make<ImplArticleList>(on_load, on_select));
}

#endif
