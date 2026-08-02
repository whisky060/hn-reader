#ifndef ARTICLELISTVIEW_H
#define ARTICLELISTVIEW_H
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include "model/DataBlock.hpp"

using namespace ftxui;

class ArticleEntry : public ComponentBase
{
public:
    ArticleEntry(ArticleDataBlock article) : article_(article) {};
    Element Render()
    {
        auto content = hbox({
            vbox({
                text(article_.title_),
                separator(),
                hbox({
                    text(article_.author_),
                    separator(),
                    text(std::to_string(article_.time_)),
                    separator(),
                    text(std::to_string(article_.score_)),
                    separator(),
                    text(std::to_string(article_.descendants_))
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
    ArticleDataBlock article_;
};

Component ArticleList(std::shared_ptr<std::vector<ArticleDataBlock>> articles)
{
    class ImplArticleList : public ComponentBase
    {
    public:
        ImplArticleList(std::shared_ptr<std::vector<ArticleDataBlock>> articles) {
            articles_ = Container::Vertical({});
            for(auto& article : *articles){
                articles_->Add(Make<ArticleEntry>(article));
            }
        };
        Element Render()
        {
            return articles_->Render();
        }

    private:
       Component articles_;
    };

    return Make<ImplArticleList>(articles);
}

#endif