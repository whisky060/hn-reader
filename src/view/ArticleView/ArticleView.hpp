#ifndef ARTICLEVIEW
#define ARTICLEVIEW

#include "concurrency/FetchResult.hpp"
#include "ftxui/dom/elements.hpp"
#include "model/ContentBlock.hpp"
#include "model/app_model.hpp"
#include "widgets/Scroll/ScrollView.hpp"
#include "widgets/Spinner/patterns.hpp"
#include <exception>
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp> // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/component/event.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/flexbox_config.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "utility/utility.h"
#include "view/BaseView.hpp"

enum class ArticleState
{
  Ready,
  Loading,
  Error
};

class ArticleViewImp : public BaseView {
public:
  ArticleViewImp(
      std::function<void(ArticleCallback)> request,
      std::function<void()> exitCallback,
      std::string articleUrl) {


    exitCallback_ = exitCallback;
    articleContent_ = nullptr;
    articleUrl_ = articleUrl;
    state = ArticleState::Loading;

    loadingSpinner_ = Spinner(spinner_patterns::HnLoading, 1000ms);
    this->Add(loadingSpinner_);

    request([this](FetchResult<ContentBlock> res){
        if(res.OK())
        {
          this->CreateArticleContent(res.GetValue());
          this->state = ArticleState::Ready;
        }
        else {
        {
          this->state = ArticleState::Error;
          std::rethrow_exception(res.GetException());
        }
        }
    });
  };
  ~ArticleViewImp() = default;

  std::string GetTitle() const override {
    return articleUrl_.empty() ? "Article" : articleUrl_;
  }

  std::vector<Shortcut> GetShortcuts() const override {
    return {
        {"Esc", "back"},
        {"Ctrl-O", "browser"},
        {"Up/Down j/k", "scroll"},
        {"PgUp/PgDn", "page"},
    };
  }

  ftxui::Element OnRender() override {
    if(state == ArticleState::Loading)
    {
      return loadingSpinner_->Render() | ftxui::center;
    }

    if(state ==  ArticleState::Ready)
    {
      return articleContent_->Render() | ftxui::vscroll_indicator;
    }

    return ftxui::text("");
  };

  bool Focusable() const override { return true; }

  bool OnEvent(ftxui::Event event) override {

    if(event == ftxui::Event::CtrlO) {
       utils::OpenUrl(articleUrl_);
       return true;
    }

    if (event == ftxui::Event::Escape) {
      exitCallback_();
      return true;
    }

    return ftxui::ComponentBase::OnEvent(event);
  }

private:
  void CreateArticleContent(ContentBlock content) {

    articleContent_ = ScrollView(CreateFromBlock(content));
    this->Add(articleContent_);
  };

  ftxui::Element CreateFromBlock(ContentBlock &block) {

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
    case ContentBlockType::TD:
      return VisitTD(block);
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
    case ContentBlockType::SPAN:
      return VisitSPAN(block);
    default:
      break;
    }
    return ftxui::text("");
  };


  ftxui::Element VisitTD(ContentBlock &block) {
    return RenderChildren(block) | ftxui::borderLight;
  };

  ftxui::Element VisitSPAN(ContentBlock &block) {
    return RenderChildren(block);
  };

  ftxui::Element VisitUL(ContentBlock &block, int depth = 0) {

    ftxui::Elements rows;

    for (auto &child : block.children) {
      rows.push_back(VisitLI(child, depth));
    }

    return ftxui::vbox(rows);
  };
  ftxui::Element VisitLI(ContentBlock &block, int depth = 0) {

    ftxui::Elements inLineBuffer;
    ftxui::Elements block_out;
    auto flush = [&] {
      if (inLineBuffer.empty())
        return;
      for (auto &e : inLineBuffer)
        e |= ftxui::xflex_shrink;
      block_out.push_back(ftxui::flexbox(inLineBuffer, ftxui::FlexboxConfig(
          {
              .direction = ftxui::FlexboxConfig::Direction::Row,
              .wrap = ftxui::FlexboxConfig::Wrap::Wrap,
          })
          .SetGap(1, 0)));
      inLineBuffer.clear();
    };

    for (auto &child : block.children) {
      if (child.type == ContentBlockType::UL) {
        auto table = VisitUL(child, depth + 1);
        block_out.push_back(table);
        continue;
      }
      if (!IsInNewLine(child.type)) {
        inLineBuffer.push_back(CreateFromBlock(child));
        continue;
      }
      flush();
      block_out.push_back(CreateFromBlock(child));
    }
    flush();
    return ftxui::flexbox(block_out, ftxui::FlexboxConfig(
        {
            .direction = ftxui::FlexboxConfig::Direction::Row,
            .wrap = ftxui::FlexboxConfig::Wrap::Wrap,
        })
        .SetGap(1, 0));
  };

  bool IsInNewLine(ContentBlockType tag) {
    return (tag == ContentBlockType::P || tag == ContentBlockType::IMG ||
            tag == ContentBlockType::TABLE || tag == ContentBlockType::H1 ||
            tag == ContentBlockType::H2 || tag == ContentBlockType::H3 ||
            tag == ContentBlockType::BR || tag == ContentBlockType::HR ||
            tag == ContentBlockType::UL ||
            tag == ContentBlockType::DIV || tag == ContentBlockType::PRE ||
            tag == ContentBlockType::BLOCKQUOTE ||
            tag == ContentBlockType::CAPTION || tag == ContentBlockType::TR);
  }

  ftxui::Element RenderChildren(ContentBlock &block) {
    ftxui::Elements inline_buf;
    ftxui::Elements block_out;

    auto flush = [&] {
      if (inline_buf.empty())
        return;
      for (auto &e : inline_buf)
        e |= ftxui::xflex_shrink;
      block_out.push_back(
      	ftxui::hflow(ftxui::hbox(std::move(inline_buf)))
      );
      inline_buf.clear();
    };

    for (auto &c : block.children) {
      if (!IsInNewLine(c.type)) {
        inline_buf.push_back(CreateFromBlock(c));
      } else {
        flush();
        block_out.push_back(CreateFromBlock(c));
      }
    }
    flush();
    return vbox(std::move(block_out));
  }

  ftxui::Element VisitBR(ContentBlock &block) { return ftxui::separatorEmpty(); }

  ftxui::Element VisitHR(ContentBlock &block) {
    return ftxui::separatorLight();
  }

  ftxui::Element VisitH1(ContentBlock &block) {
    return ftxui::hflow(ftxui::hbox(
        {ftxui::text("✺ "), RenderChildren(block), ftxui::text(" ✺")}));
  };

  ftxui::Element VisitH2(ContentBlock block) {
    return ftxui::hflow(ftxui::flexbox(
        {ftxui::text("❊ "), RenderChildren(block), ftxui::text(" ❊")}));
  };

  ftxui::Element VisitH3(ContentBlock &block) {
    return ftxui::hflow(ftxui::flexbox(
        {ftxui::text("❃ "), RenderChildren(block), ftxui::text(" ❃")}));
  };

  ftxui::Element VisitP(ContentBlock &block) { return RenderChildren(block); }

  ftxui::Element VisitTEXT(ContentBlock &block) {
    return ftxui::paragraph(block.text) | ftxui::notflex;
  }

  ftxui::Element VisitDIV(ContentBlock &block) { return RenderChildren(block); }

  ftxui::Element VisitA(ContentBlock &block) {
    return RenderChildren(block) | ftxui::hyperlink(block.url) | ftxui::notflex;
  };

  ftxui::Element VisitTABLE(ContentBlock &block) {
    return RenderChildren(block) | ftxui::border;
  }

  ftxui::Element VisitTR(ContentBlock &block) {
    return RenderChildren(block) | ftxui::border;
  }

  ftxui::Element VisitPRE(ContentBlock &block) {
    return RenderChildren(block);
  }

  ftxui::Element VisitB(ContentBlock &block) {
    return RenderChildren(block) | ftxui::bold;
  }

  ftxui::Element VisitI(ContentBlock &block) {
    return RenderChildren(block) | ftxui::italic;
  }

  ftxui::Element VisitBLOCKQUOTE(ContentBlock &block) {
    return RenderChildren(block) | ftxui::border | ftxui::center;
  }

  ftxui::Element VisitU(ContentBlock &block) {
    return RenderChildren(block) | ftxui::underlined;
  }

  ftxui::Element VisitCODE(ContentBlock &block) {
    return RenderChildren(block);
  }

  ftxui::Element VisitIMG(ContentBlock &block) {
    return ftxui::text("📸") | ftxui::hyperlink(block.url) | ftxui::notflex;
  }

  ftxui::Element VisitCAPTION(ContentBlock &block) {
    return RenderChildren(block);
  }


  ArticleState state;
  std::function<void()> exitCallback_;
  ftxui::Component loadingSpinner_;
  ftxui::Component articleContent_;
  std::vector<std::string> listDots_ = {"•", "◦", "▪"};
  std::vector<ftxui::Component> tempComponents_;
  std::string articleUrl_;
};

inline std::shared_ptr<BaseView>
ArticleView(std::function<void(ArticleCallback)> request,
            std::function<void()> exitCallback,
            std::string articleUrl) {
  return std::static_pointer_cast<BaseView>(
      ftxui::Make<ArticleViewImp>(request, exitCallback, articleUrl));
}

#endif
