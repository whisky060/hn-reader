#ifndef ARTICLELISTVIEW.hpp
#define ARTICLELISTVIEW .hpp
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include "model/DataBlock.hpp"

using namespace ftxui;

class ArticleEntry : public ComponentBase
{
public:
    ArticleEntry(std::shared_ptr<ArticleDataBlock> article) : article_(article) {};
    Element Render()
    {
        auto content = hbox({
            vbox({
                text(article_->title_),
                separator(),
                hbox({
                    text(article_->author_),
                    separator(),
                    text(std::to_string(article_->time_)),
                    separator(),
                    text(std::to_string(article_->score_)),
                    separator(),
                    text(std::to_string(article_->descendants_))
                })
            }),
        });

        // Add visual cues when highlighted by keyboard navigation
        if (Focused())
        {
            content = content | inverted;
        }
        return content;
    }

private:
    std::shared_ptr<ArticleDataBlock> article_;
};

Component ArticleList()
{
    class ImplArticleList : public ComponentBase
    {
    public:
        ImplArticleList(std::shared_ptr<std::vector<ArticleDataBlock>> articles) {
            for(auto& article : *articles){
                    articles_->push_back(Make<ArticleEntry>(article));                               
            }
        };
        Element Render()
        {
            auto content =  vbox(articles_);
            
            

            return content;
        }

    private:
        std::shared_ptr<std::vector<Component>> articles_;
    };

    return Make<ImplArticleList>();
}

#endif