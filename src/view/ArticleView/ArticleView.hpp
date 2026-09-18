#ifndef ARTICLEVIEW
#define ARTICLEVIEW


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

            switch(block.type)
            {
                case ContentBlockType::H1:
                    return ftxui::Renderer([&block]{
                        return ftxui::text(block.text + "\n") | ftxui::center | ftxui::bold;
                    });
                case ContentBlockType::H2:
                    return ftxui::Renderer([&block]{
                        return ftxui::text(block.text + "\n") | ftxui::center;
                    });
                case ContentBlockType::H3:
                    return ftxui::Renderer([&block]{
                        return ftxui::text(block.text + "\n") | ftxui::underlined;
                    });
                case ContentBlockType::P:
                    break;
                case ContentBlockType::TEXT:
                    break;
                case ContentBlockType::A:
                    break;
                case ContentBlockType::TABLE:
                    break;
                case ContentBlockType::TABLEROW:
                    break;
                case ContentBlockType::PRE:
                    break;
                case ContentBlockType::B:
                    break;
                case ContentBlockType::I:
                    break;
                case ContentBlockType::EM:
                    break;
                case ContentBlockType::BLOCKQUOTE:
                    break;
                case ContentBlockType::CODE:
                    break;
                case ContentBlockType::IMG:
                    break;
                case ContentBlockType::LI:
                    break;
                case ContentBlockType::CAPTION:
                    break;
                default:
                    break;
            }
        };
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
};



inline ftxui::Component ArticleView(std::function<std::shared_ptr<std::vector<ContentBlock>>()> articleContentGetter) {
    return ftxui::Make<ArticleViewImp>(articleContentGetter);
}


#endif
