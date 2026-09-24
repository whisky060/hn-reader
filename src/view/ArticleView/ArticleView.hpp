#ifndef ARTICLEVIEW
#define ARTICLEVIEW

#include "ftxui/dom/elements.hpp"
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
  ArticleViewImp(std::function<std::shared_ptr<ContentBlock>()> articleContentGetter)
  {

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

  bool Focusable() const override { return true; }


  bool OnEvent(ftxui::Event event) override {

  		if(event == ftxui::Event::Escape)
    	{
     		return true;
     	}

  		return ftxui::ComponentBase::OnEvent(event);
  }

private:
  void CreateArticleContent() {
    auto content = articleContentGetter_();
    if (content == nullptr)
      return;

    articleContent_ = CreateComponentFromBlock(*content);
  };

  ftxui::Element KeepAlive(ftxui::Component component) {
    tempComponents_.push_back(component);
    return component->Render();
  };

  ftxui::Component CreateComponentFromBlock(ContentBlock &block) {

    std::vector<ftxui::Component> components;
    switch (block.type) {
    case ContentBlockType::BR:
      return VisitBR(block);
    case ContentBlockType::HR:
      return VisitHR(block);
    case ContentBlockType::H1:
      return VisitH1(block);
    case ContentBlockType::H2:
      return VisitH2(block);
    case ContentBlockType::H3:
      return VisitH3(block);
    case ContentBlockType::P:
      return VisitP(block);
    case ContentBlockType::TEXT:
      return VisitTEXT(block);
    case ContentBlockType::DIV:
      return VisitDIV(block);
    case ContentBlockType::A:
      return VisitA(block);
    case ContentBlockType::TABLE:
      return VisitTABLE(block);
    case ContentBlockType::TR:
      return VisitTR(block);
    case ContentBlockType::PRE:
      return VisitPRE(block);
    case ContentBlockType::B:
      return VisitB(block);
    case ContentBlockType::I:
      return VisitI(block);
    case ContentBlockType::UL:
      return VisitUL(block);
    case ContentBlockType::BLOCKQUOTE:
      return VisitBLOCKQUOTE(block);
    case ContentBlockType::U:
      return VisitU(block);
    case ContentBlockType::CODE:
      return VisitCODE(block);
    case ContentBlockType::IMG:
      return VisitIMG(block);
    case ContentBlockType::CAPTION:
      return VisitCAPTION(block);
    default:
      break;
    }
    return ftxui::Renderer([] { return ftxui::text("") | ftxui::notflex; });
  };

  ftxui::Component VisitUL(ContentBlock &block, int depth = 0) {

    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      auto li = VisitLI(child, depth);
      mainContainer->Add(ftxui::Renderer([this, li, depth] {
        return ftxui::hbox(
            {
            ftxui::text(" " + listDots_[depth % listDots_.size()]),
            li->Render()
        }
        );
      }));
    }

    return mainContainer;
  };
  ftxui::Component VisitLI(ContentBlock &block, int depth = 0) {

    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (child.type == ContentBlockType::UL) {
        auto table = VisitUL(child, depth + 1);
        mainContainer->Add(table);
        continue;
      }
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child)));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child)));
    }
    auto line = FlushBuffer(inLineBuffer);
    mainContainer->Add(line);
    return ftxui::Renderer([mainContainer] { return mainContainer->Render(); });
  };

  bool IsInNewLine(ContentBlockType tag) {
    return (tag == ContentBlockType::P || tag == ContentBlockType::IMG ||
            tag == ContentBlockType::TABLE || tag == ContentBlockType::H1 ||
            tag == ContentBlockType::H2 || tag == ContentBlockType::H3 ||
            tag == ContentBlockType::BR || tag == ContentBlockType::HR ||
            tag == ContentBlockType::CODE || tag == ContentBlockType::UL);
  }

  ftxui::Component VisitBR(ContentBlock &block) {
    return ftxui::Renderer([] { return ftxui::separator() ;    });
  }

  ftxui::Component VisitHR(ContentBlock &block) {
    return ftxui::Renderer([] { return ftxui::separatorDashed() ; });
  }

  ftxui::Component VisitH1(ContentBlock &block) {

    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
    }
    auto line = FlushBuffer(inLineBuffer);
    mainContainer->Add(line);
    return ftxui::Renderer([mainContainer] {
      return ftxui::flexbox(
          {ftxui::text("✺ "),
           mainContainer->Render() | ftxui::center | ftxui::bold ,
           ftxui::text(" ✺")},
          ftxui::FlexboxConfig({

          	.direction = ftxui::FlexboxConfig::Direction::Row,
           	.wrap = ftxui::FlexboxConfig::Wrap::Wrap,
          }).SetGap(1, 0));
    });
  };

  ftxui::Component VisitH2(ContentBlock block) {
    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
    }
    auto line = FlushBuffer(inLineBuffer);
    mainContainer->Add(line);

    return ftxui::Renderer([mainContainer] {
      return ftxui::flexbox({ftxui::text("❊ "),
                             mainContainer->Render() | ftxui::center ,
                             ftxui::text(" ❊")},
                            ftxui::FlexboxConfig({
                            	.direction = ftxui::FlexboxConfig::Direction::Row,
                             	.wrap = ftxui::FlexboxConfig::Wrap::Wrap,
                            }).SetGap(1, 0));
    });
  };

  ftxui::Component VisitH3(ContentBlock &block) {

    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
    }
    auto line = FlushBuffer(inLineBuffer);
    mainContainer->Add(line);
    return ftxui::Renderer([mainContainer] {
      return ftxui::flexbox(
          {ftxui::text("❃ "), mainContainer->Render() | ftxui::underlined },
          ftxui::FlexboxConfig({

          		.direction = ftxui::FlexboxConfig::Direction::Row,
            	.wrap = ftxui::FlexboxConfig::Wrap::Wrap,
          }).SetGap(1, 0));
    });
  };

  ftxui::Component VisitP(ContentBlock &block) {
    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
    }
    auto line = FlushBuffer(inLineBuffer);
    mainContainer->Add(line);
    return ftxui::Renderer([mainContainer] { return mainContainer->Render() ; });
  }

  ftxui::Component VisitTEXT(ContentBlock &block) {
    return ftxui::Renderer([block] { return ftxui::paragraph(block.text) ; });
  }

  ftxui::Component VisitDIV(ContentBlock &block) {
    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
    }
    auto line = FlushBuffer(inLineBuffer);
    mainContainer->Add(line);
    return ftxui::Renderer([mainContainer] { return mainContainer->Render() ; });
  }

  ftxui::Component VisitA(ContentBlock &block) {
    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
    }
    auto line = FlushBuffer(inLineBuffer);
    mainContainer->Add(line);
    return ftxui::Renderer([mainContainer, url = block.url] {
      return mainContainer->Render() | ftxui::hyperlink(url) ;
    });
  };

  ftxui::Component VisitTABLE(ContentBlock &block) {
    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
    }
    auto line = FlushBuffer(inLineBuffer);
    mainContainer->Add(line);
    return ftxui::Renderer(
        [mainContainer] { return mainContainer->Render() | ftxui::border ; });
  }

  ftxui::Component VisitTR(ContentBlock &block) {
    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ) |
                             ftxui::border);
    }
    auto line = FlushBuffer(inLineBuffer);
    mainContainer->Add(line);
    return ftxui::Renderer(
        [mainContainer] { return mainContainer->Render() | ftxui::border; });
  }

  ftxui::Component VisitPRE(ContentBlock &block) {
    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
    }

    return ftxui::Renderer([mainContainer] {
      return mainContainer->Render() | ftxui::center | ftxui::notflex;
    });
  };

  ftxui::Component VisitB(ContentBlock &block) {
    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
    }
    auto line = FlushBuffer(inLineBuffer);
    mainContainer->Add(line);
    return ftxui::Renderer(
        [mainContainer] { return mainContainer->Render() | ftxui::bold ; });
  };

  ftxui::Component VisitI(ContentBlock &block) {
    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
    }
    auto line = FlushBuffer(inLineBuffer);
    mainContainer->Add(line);
    return ftxui::Renderer([mainContainer, url = block.url] {
      return mainContainer->Render() | ftxui::italic ;
    });
  };

  ftxui::Component VisitBLOCKQUOTE(ContentBlock &block) {
    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
    }

    auto line = FlushBuffer(inLineBuffer);
    mainContainer->Add(line);

    return ftxui::Renderer([mainContainer] {
      return mainContainer->Render() | ftxui::border | ftxui::center |
             ftxui::notflex;
    });
  }

  ftxui::Component VisitU(ContentBlock &block) {
    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
    }
    auto line = FlushBuffer(inLineBuffer);
    mainContainer->Add(line);
    return ftxui::Renderer([mainContainer, url = block.url] {
      return mainContainer->Render() | ftxui::underlined;
    });
  }

  ftxui::Component VisitCODE(ContentBlock &block) {
    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
    }
    auto line = FlushBuffer(inLineBuffer);
    mainContainer->Add(line);
    return ftxui::Renderer([mainContainer, url = block.url] {
      return mainContainer->Render();
    });
  }

  ftxui::Component VisitIMG(ContentBlock &block) {
    return ftxui::Renderer([url = block.url] {
      return ftxui::text("📸") | ftxui::hyperlink(url) | ftxui::notflex;
    });
  }

  ftxui::Component VisitCAPTION(ContentBlock &block) {
    ftxui::Elements inLineBuffer;
    auto mainContainer = ftxui::Container::Vertical({});

    for (auto &child : block.children) {
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
        continue;
      }
      auto line = FlushBuffer(inLineBuffer);
      mainContainer->Add(line);
      inLineBuffer.push_back(KeepAlive(CreateComponentFromBlock(child) ));
    }
    auto line = FlushBuffer(inLineBuffer);
    mainContainer->Add(line);
    return ftxui::Renderer(
        [mainContainer] { return mainContainer->Render() | ftxui::center; });
  }

  ftxui::Component FlushBuffer(ftxui::Elements &buffer) {


    for (auto &element : buffer) {
      element |= ftxui::xflex_shrink;
    }

    auto box = ftxui::dbox(buffer) ;

    buffer.clear();
    return ftxui::Renderer([box] { return box; });
  };
  std::function<std::shared_ptr<ContentBlock>()> articleContentGetter_;
  ftxui::Component loadingSpinner_;
  ftxui::Component articleContent_;
  std::vector<std::string> listDots_ = {"•", "◦", "▪"};
  std::vector<ftxui::Component> tempComponents_;
};

inline ftxui::Component ArticleView(std::function<std::shared_ptr<ContentBlock>()> articleContentGetter)
{
  return ftxui::Make<ArticleViewImp>(articleContentGetter);
}

#endif
