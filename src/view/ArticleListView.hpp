#ifndef ARTICLELISTVIEW_H
#define ARTICLELISTVIEW_H
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include "model/DataBlock.hpp"

class ArticleEntry : public ftxui::ComponentBase
{
public:
    ArticleEntry(StoryDataBlock article) : article_(article) {};

    ftxui::Element OnRender()
    {
        auto content = ftxui::vbox({ftxui::text(article_.title_) | ftxui::notflex,
                                    ftxui::separator(),
                                    ftxui::hbox({ftxui::text(article_.author_) | ftxui::notflex,
                                                 ftxui::separator(),
                                                 ftxui::text(std::to_string(article_.time_)) | ftxui::notflex,
                                                 ftxui::separator(),
                                                 ftxui::text(std::to_string(article_.score_)) | ftxui::notflex,
                                                 ftxui::separator(),
                                                 ftxui::text(std::to_string(article_.descendants_)) | ftxui::notflex}),
                                    ftxui::separator(),
                                    ftxui::separatorEmpty()}) | ftxui::notflex;

        // Add visual cues when highlighted by keyboard navigation
        if (Focused())
        {
            content = content | inverted;
        }
        return content;
    }

    bool OnEvent(ftxui::Event event)
    {
        
        if(ftxui::ComponentBase::OnEvent(event))
        {
            return true;
        } 

        return false;
    }

private:
    StoryDataBlock article_;
};

ftxui::Component ArticleList(std::shared_ptr<std::vector<StoryDataBlock>> articles)
{
    class ImplArticleList : public ftxui::ComponentBase
    {
    public:
        ImplArticleList(std::shared_ptr<std::vector<StoryDataBlock>> articles)
        {
            articles_ = ftxui::Container::Vertical({});
            for (auto &article : *articles)
            {
                articles_->Add(ftxui::Make<ArticleEntry>(article));
            }
            this->Add(articles_);
        };
        ftxui::Element OnRender()
        {
            return articles_->Render(); 
        }

        bool OnEvent(ftxui::Event event)
        {   

            if(ftxui::ComponentBase::OnEvent(event))
            {
                return true;
            }

            return false;
        }

    private:
        ftxui::Component articles_;
    };

    return ftxui::Make<ImplArticleList>(articles);
}

#endif