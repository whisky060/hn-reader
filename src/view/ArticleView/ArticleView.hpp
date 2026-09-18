#ifndef ARTICLEVIEW
#define ARTICLEVIEW


#include <cstdio>
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/mouse.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "model/ContentBlock.hpp"



class ArticleViewImp : public ftxui::ComponentBase {
    public:
        ArticleViewImp(std::function<std::shared_ptr<std::vector<ContentBlock>>()> articleContentGetter){

            articleContentGetter_ = articleContentGetter;
            articleContent_ = nullptr;
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
            loadingSpinner_ = Spinner(first_pattern, 1000ms);
            this->Add(loadingSpinner_);

        };
        ~ArticleViewImp();

        ftxui::Element OnRender() override
        {
            if(articleContent_ == nullptr && articleContentGetter_() == nullptr)
                return loadingSpinner_->Render();
            else if(articleContent_ == nullptr)
            {
                CreateArticleContent();
                return articleContent_->Render();
            }
            else
                return articleContent_->Render();


        };

    private:
        ftxui::Component CreateComponentFromBlock(ContentBlock& block)
        {

            std::vector<ftxui::Component> components;
            switch(block.type)
            {
                case ContentBlockType::H1:
                    return ftxui::Renderer([&block]{
                        return ftxui::text("✺ " + block.text + " ✺") | ftxui::center | ftxui::bold;
                    });
                case ContentBlockType::H2:
                    return ftxui::Renderer([&block]{
                        return ftxui::text("❊ " + block.text + " ❊") | ftxui::center;
                    });
                case ContentBlockType::H3:
                    return ftxui::Renderer([&block]{
                        return ftxui::text("❃ " + block.text) | ftxui::underlined;
                    });
                case ContentBlockType::P:
                    for(auto& child : block.children)
                    {
                        components.push_back(CreateComponentFromBlock(child));
                    }
                    return ftxui::Renderer([&components]{
                        return  ftxui::vbox(
                            ftxui::vbox(
                                components
                            ),
                            ftxui::separator()
                        );
                    });
                case ContentBlockType::TEXT:
                    return ftxui::Renderer([&block]{
                        return ftxui::text(block.text);
                    });
                case ContentBlockType::DIV:
                    for(auto& child : block.children)
                    {
                        components.push_back(CreateComponentFromBlock(child));
                    }
                    return ftxui::Renderer([&components]{
                        return ftxui::vbox(
                            components
                        );
                    });
                case ContentBlockType::A:
                    return ftxui::Renderer([&block]{
                        return ftxui::text(block.text + "[" + block.url + "]");
                    });
                case ContentBlockType::TABLE:
                    for(auto& child : block.children)
                    {
                        components.push_back(CreateComponentFromBlock(child));
                    }
                    return ftxui::Renderer([&components]{
                        return ftxui::vbox(
                            components
                        ) | ftxui::border | ftxui::center;
                    });
                case ContentBlockType::TABLEROW:
                    for(auto& child : block.children)
                    {
                        components.push_back(CreateComponentFromBlock(child));
                    }
                    return ftxui::Renderer([&components]{
                        return ftxui::vbox(
                            components
                        ) | ftxui::border;
                    });
                case ContentBlockType::PRE:
                    for(auto& child : block.children)
                    {
                        components.push_back(CreateComponentFromBlock(child));
                    }
                    return ftxui::Renderer([&components]{
                        return ftxui::vbox(
                            components
                        ); // TODO add a specific text color?
                    });
                case ContentBlockType::B:
                    for(auto& child : block.children)
                    {
                        components.push_back(CreateComponentFromBlock(child));
                    }
                    return ftxui::Renderer([&components]{
                        return ftxui::vbox(
                            components
                        ) | ftxui::bold;
                    });
                case ContentBlockType::I:
                    for(auto& child : block.children)
                    {
                        components.push_back(CreateComponentFromBlock(child));
                    }
                    return ftxui::Renderer([&components]{
                        return ftxui::vbox(
                            components
                        ) | ftxui::italic;
                    });
                case ContentBlockType::UL:
                    return CreateListItem(block, 0);
                case ContentBlockType::BLOCKQUOTE:
                    for(auto& child : block.children)
                    {
                        components.push_back(CreateComponentFromBlock(child));
                    }
                    return ftxui::Renderer([&components]{
                        return ftxui::vbox(
                            components
                        ) | ftxui::border | ftxui::center;
                    });
                case ContentBlockType::U:
                    for(auto& child : block.children)
                    {
                        components.push_back(CreateComponentFromBlock(child));
                    }
                    return ftxui::Renderer([&components]{
                        return ftxui::vbox(
                            components
                        ) | ftxui::underlined;
                    });
                case ContentBlockType::CODE:
                    for(auto& child : block.children)
                    {
                        components.push_back(CreateComponentFromBlock(child));
                    }
                    return ftxui::Renderer([&components]{
                        return ftxui::vbox(
                            components
                        ) | ftxui::border | ftxui::center;
                    });
                case ContentBlockType::IMG:
                    break;
                case ContentBlockType::CAPTION:
                    for(auto& child : block.children)
                    {
                        components.push_back(CreateComponentFromBlock(child));
                    }
                    return ftxui::Renderer([&components]{
                        return ftxui::vbox(
                            components
                        ) | ftxui::center;
                    });
                default:
                    break;
            }
        };
        ftxui::Component CreateListItem(ContentBlock& block, int depth)
        {
            std::vector<ftxui::Component> components;
            for(auto& child : block.children)
            {
                switch (child.type) {
                    case ContentBlockType::UL:
                        components.push_back(CreateListItem(child, depth+1));
                        break;
                    default:
                        components.push_back(CreateComponentFromBlock(child));
                        break;
                }
            }

        }
        void CreateArticleContent(){
            auto articleAST = articleContentGetter_();
            if(articleAST == nullptr)
                return;

            articleContent_ = ftxui::Container::Vertical({});

            for(auto& block : *articleAST)
            {
                auto elem = CreateComponentFromBlock(block);
                articleContent_->Add(elem);
            }

        };
        std::function<std::shared_ptr<std::vector<ContentBlock>>()> articleContentGetter_;
        ftxui::Component loadingSpinner_;
        ftxui::Component articleContent_;
        std::vector<std::string> listDots_ = {"•", "◦", "▪"};
};



inline ftxui::Component ArticleView(std::function<std::shared_ptr<std::vector<ContentBlock>>()> articleContentGetter) {
    return ftxui::Make<ArticleViewImp>(articleContentGetter);
}


#endif
