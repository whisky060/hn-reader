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
#include "view/ArticleList/ArticleEntry.hpp"

ftxui::Component ArticleList(std::shared_ptr<std::vector<StoryDataBlock>> articles,
                             std::function<void(StoryDataBlock)> on_select,
                             std::function<void()> on_load,
                             std::function<bool()> is_loading
                            )
{

    class ImplArticleList : public ftxui::ComponentBase
    {
    public:
        ImplArticleList(std::shared_ptr<std::vector<StoryDataBlock>> articles,
                        std::function<void()> on_load,
                        std::function<void(StoryDataBlock)> on_select,
                        std::function<bool()> is_loading
                        ) : ftxui::ComponentBase()
        {
            articles_ = articles;
            on_load_ = on_load;
            on_select_ = on_select;
            is_loading_ = is_loading;
            
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
                    return content | ftxui::inverted | ftxui::center | ftxui::border;
                }
                return content | ftxui::center | ftxui::border;
            };

            auto load_more_button = ftxui::Button("Load More", on_load_, button_options);
            load_more_button_ = load_more_button;
            this->Add(list_);
            list_->Add(load_more_button_);
        }
        

        ftxui::Element OnRender() override
        {   

            if(is_loading_())
            {
                 // TODO make the loading row render
            }   
                

            if(old_state==false and is_loading_()==false)
            {
                Refresh();
            }
            old_state = is_loading_();

            return list_->Render();
        }

    private:
        void Refresh()
        {

            list_->Detach();
            entries_.clear();
            int indx = list_->ChildCount() ;
            for (auto &article : *articles_)
            {
                entries_.push_back(ftxui::Make<ArticleEntry>(article, on_select_));
            }
            list_ = ftxui::Container::Vertical(entries_);
            if(articles_->size()<500)
                list_->Add(load_more_button_);
            
            this->Add(list_);
            // auto child = list_->ChildAt(indx);
            //child->TakeFocus();
           
      
        }

        ftxui::Component load_more_button_;
        ftxui::Component list_;
        std::shared_ptr<std::vector<StoryDataBlock>> articles_;
        std::vector<ftxui::Component> entries_;
        std::function<void()> on_load_;
        std::function<void(StoryDataBlock)> on_select_;
        std::function<bool()> is_loading_;
        bool old_state;
    };
    
    return ftxui::Make<ImplArticleList>(articles,on_load, on_select, is_loading);
}

#endif