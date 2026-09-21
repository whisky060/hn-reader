#ifndef ARTICLEVIEW
#define ARTICLEVIEW

#include "model/ContentBlock.hpp"
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp> // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/component/event.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class ArticleViewImp : public ftxui::ComponentBase {
public:
  ArticleViewImp(std::function<std::shared_ptr<std::vector<ContentBlock>>()>
                     articleContentGetter) {

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
  ~ArticleViewImp() = default;

  ftxui::Element OnRender() override {
    if (articleContent_ == nullptr && articleContentGetter_() == nullptr)
      return loadingSpinner_->Render() | ftxui::center;
    else if (articleContent_ == nullptr) {
      CreateArticleContent();
      return articleContent_->Render() | ftxui::vscroll_indicator;
    } else
      return articleContent_->Render() | ftxui::vscroll_indicator;
  };

  bool Focusable() const override
  {
      return true;
  }

private:
  ftxui::Element CreateComponentFromBlock(ContentBlock &block) {

    std::vector<ftxui::Component> components;
    switch (block.type) {
    case ContentBlockType::BR:
      return ftxui::separator();
    case ContentBlockType::HR:
      return ftxui::separatorLight();
    case ContentBlockType::H1:
      return ftxui::flexbox({ftxui::text("✺ "), Flatten(block.children),
                             ftxui::text(" ✺")},
                            ftxui::FlexboxConfig().SetGap(1, 0)) | ftxui::center | ftxui::bold;
    case ContentBlockType::H2:
      return ftxui::flexbox({ftxui::text("❊ "), Flatten(block.children),
                             ftxui::text(" ❊")},
                            ftxui::FlexboxConfig().SetGap(1, 0)) | ftxui::center ;
    case ContentBlockType::H3:
      return ftxui::flexbox({ftxui::text("❃ "), Flatten(block.children)},
                            ftxui::FlexboxConfig().SetGap(1, 0)) | ftxui::underlined ;
    case ContentBlockType::P:
      return ftxui::vbox(Flatten(block.children), ftxui::separator());
    case ContentBlockType::TEXT:
      return ftxui::paragraph(block.text);
    case ContentBlockType::DIV:
      return Flatten(block.children);
    case ContentBlockType::A:
      return Flatten(block.children) | ftxui::hyperlink(block.url);
    case ContentBlockType::TABLE:
      return ftxui::vbox(Flatten(block.children)) |ftxui::border; // TODO
    case ContentBlockType::TR:
      return Flatten(block.children);
    case ContentBlockType::PRE:
      return Flatten(block.children);
    case ContentBlockType::B:
      return Flatten(block.children) | ftxui::bold;
    case ContentBlockType::I:
      return Flatten(block.children) | ftxui::italic;
    case ContentBlockType::UL:
      return CreateListItem(block, 0);
    case ContentBlockType::BLOCKQUOTE:
      return Flatten(block.children) | ftxui::border | ftxui::center | ftxui::notflex;
    case ContentBlockType::U:
      return Flatten(block.children) | ftxui::underlined | ftxui::notflex;
    case ContentBlockType::CODE:
      return Flatten(block.children) | ftxui::center | ftxui::border | ftxui::notflex;
    case ContentBlockType::IMG:
      return ftxui::text("📸") | ftxui::hyperlink(block.url) | ftxui::notflex;
    case ContentBlockType::CAPTION:
      return Flatten(block.children) | ftxui::center;
    default:
      break;
    }
    return ftxui::text("") | ftxui::notflex;
  };
  ftxui::Element CreateListItem(ContentBlock &block, int depth = 0) {

    std::vector<ftxui::Element> elements;
    for (auto &child : block.children) {

      if (child.type != ContentBlockType::LI)
        continue;

      for (auto &grandchild : child.children) {
        if (grandchild.type == ContentBlockType::UL) {
          elements.push_back(
          	ftxui::hbox(
           		ftxui::text(" " + listDots_[depth % listDots_.size()] + " "),
          		CreateListItem(grandchild, depth + 1)
           )
          );
        } else {
          elements.push_back(
          	ftxui::hbox(
           		ftxui::text(" " + listDots_[depth % listDots_.size()] + " "),
             	CreateComponentFromBlock(grandchild)
           )
          );
        }
      }
    }
    return ftxui::flexbox(std::move(elements),
                          ftxui::FlexboxConfig({
                          	.direction = ftxui::FlexboxConfig::Direction::Row,
                          	.wrap = ftxui::FlexboxConfig::Wrap::Wrap,
                          }).SetGap(1, 0));
  };
  void CreateArticleContent() {
    auto articleAST = articleContentGetter_();
    if (articleAST == nullptr)
      return;

    articleContent_ = ftxui::Container::Vertical({});

    for (auto &block : *articleAST) {
      auto elem = CreateComponentFromBlock(block);
      articleContent_->Add(ftxui::Renderer([elem] { return elem; }));
    }
    this->Add(articleContent_);
  };

  ftxui::Element Flatten(std::vector<ContentBlock> &blocks) {
    std::vector<ftxui::Element> elements;
    for (auto &block : blocks) {
      elements.push_back(CreateComponentFromBlock(block));
    }
    return ftxui::flexbox(std::move(elements),
                          ftxui::FlexboxConfig().SetGap(1, 0));
  };

  std::function<std::shared_ptr<std::vector<ContentBlock>>()> articleContentGetter_;
  ftxui::Component loadingSpinner_;
  ftxui::Component articleContent_;
  std::vector<std::string> listDots_ = {"•", "◦", "▪"};
};

inline ftxui::Component
ArticleView(std::function<std::shared_ptr<std::vector<ContentBlock>>()>
                articleContentGetter) {
  return ftxui::Make<ArticleViewImp>(articleContentGetter);
}

#endif
