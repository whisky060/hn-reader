#ifndef ARTICLEVIEW
#define ARTICLEVIEW

#include "concurrency/FetchResult.hpp"
#include "ftxui/dom/elements.hpp"
#include "model/ContentBlock.hpp"
#include "model/DataBlock.hpp"
#include "model/app_model.hpp"
#include "widgets/Scroll/ScrollView.hpp"
#include "widgets/Spinner/patterns.hpp"
#include <codecvt>
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
#include <string_view>
#include <vector>
#include "utility/utility.h"
#include "view/BaseView.hpp"

enum class ArticleState
{
  Ready,
  Loading,
  Error
};

struct WalkState
{
  ftxui::Elements inline_;
  ftxui::Elements blocks_;
  int listDepth_ = 0;

  void Flush()
  {
    if(inline_.empty())
      return;
    blocks_.push_back(ftxui::hflow(ftxui::hbox(std::move(inline_))));
    inline_.clear();
  }

  ftxui::Element Finish()
  {
    Flush();
    return ftxui::vbox(std::move(blocks_));
  }

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
    state_ = ArticleState::Loading;

    loadingSpinner_ = Spinner(spinner_patterns::HnLoading, 1000ms);
    this->Add(loadingSpinner_);

    request([this](FetchResult<ContentBlock> res){
        if(res.OK())
        {
          this->CreateArticleContent(res.GetValue());
          this->state_ = ArticleState::Ready;
        }
        else {
        {
          this->state_ = ArticleState::Error;
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
    if(state_ == ArticleState::Loading)
    {
      return loadingSpinner_->Render() | ftxui::center;
    }

    if(state_ ==  ArticleState::Ready)
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

    articleContent_ = ScrollView(WalkBlock(content));
    this->Add(articleContent_);
  };



  bool IsInNewLine(ContentBlockType tag) {
    return (tag == ContentBlockType::P ||
            tag == ContentBlockType::TABLE || tag == ContentBlockType::H1 ||
            tag == ContentBlockType::H2 || tag == ContentBlockType::H3 ||
            tag == ContentBlockType::BR || tag == ContentBlockType::HR ||
            tag == ContentBlockType::UL ||
            tag == ContentBlockType::DIV || tag == ContentBlockType::PRE ||
            tag == ContentBlockType::BLOCKQUOTE ||
            tag == ContentBlockType::CAPTION || tag == ContentBlockType::TR);
  }


  ftxui::Element WalkBlock(ContentBlock& block)
  {
    WalkState state;
    ftxui::Decorator dec = ftxui::nothing;
    switch (block.type)
    {
      case ContentBlockType::H1:
        dec = dec | ftxui::bold | ftxui::underlined;
        WalkChildren(block,state,dec);
        return state.Finish();
      case ContentBlockType::H2:
        dec = dec | ftxui::bold;
        WalkChildren(block,state,dec);
        return state.Finish();
      case ContentBlockType::H3:
        WalkChildren(block,state);
        return state.Finish();
      case ContentBlockType::P:
        WalkChildren(block,state);
        return ftxui::vbox({
          ftxui::separatorEmpty(),
          state.Finish()
        });
      case ContentBlockType::DIV:
        WalkChildren(block,state);
        return state.Finish();
      case ContentBlockType::BR:
        return ftxui::separatorEmpty();
      case ContentBlockType::HR:
        return ftxui::separatorLight();
      case ContentBlockType::BLOCKQUOTE:
        WalkChildren(block,state);
        return state.Finish() | ftxui::center | ftxui::borderLight;
      case ContentBlockType::PRE:
        WalkChildren(block,state);
        return state.Finish();
      case ContentBlockType::CAPTION:
        WalkChildren(block,state);
        return state.Finish() | ftxui::center;
      case ContentBlockType::TABLE:
        return VisitTABLE(block);
      case ContentBlockType::UL:
        return VisitUL(block,0);
      default:
        WalkChildren(block,state);
        return state.Finish();
    }
  }


  ftxui::Element VisitTABLE(ContentBlock& block)
  {
    ftxui::Elements rows;
    for(auto& child : block.children)
    {
      if(child.type == ContentBlockType::CAPTION)
      {
        WalkState state;
        WalkChildren(block,state);
        rows.push_back(state.Finish() | ftxui::center);
      }
      else if(child.type == ContentBlockType::TR)
      {
        rows.push_back(VisitTR(child));
      }
    }

    return ftxui::vbox(rows) | ftxui::border | ftxui::center;
  }

  ftxui::Element VisitTR(ContentBlock& block)
  {

    ftxui::Elements col;
    for(auto& child : block.children)
    {
      col.push_back(WalkBlock(child) | ftxui::border);
    }
    return ftxui::hbox(col) | ftxui::border ;

  }

  ftxui::Element VisitUL(ContentBlock& block, int depth)
  {
    ftxui::Elements rows;
      for (auto& child : block.children) rows.push_back(VisitLI(child, depth));
    return ftxui::vbox(std::move(rows));
  }

  ftxui::Element VisitLI(
    ContentBlock& block,
    int depth
  )
  {
    WalkState state;
    state.listDepth_ = depth;
    for (auto& child : block.children) {
      if (child.type == ContentBlockType::UL) {
        state.Flush();
        state.blocks_.push_back(VisitUL(child, depth + 1));
      } else if (IsInNewLine(child.type)) {
        state.Flush();
        state.blocks_.push_back(WalkBlock(child));
      } else {
        WalkInLine(child,state);
      }
    }
    return  state.Finish();
  }

  void WalkInLine(
    ContentBlock& block,
    WalkState& state,
    ftxui::Decorator dec = ftxui::nothing
  )
  {
    switch (block.type) {
      case ContentBlockType::TEXT:
        PushText(block.text,state,dec);
        break;
      case ContentBlockType::A:
        dec = dec | ftxui::hyperlink(block.url);
        WalkChildren(block,state,dec);
        break;
      case ContentBlockType::I:
        dec = dec | ftxui::italic;
        WalkChildren(block,state,dec);
        break;
      case ContentBlockType::U:
        dec = dec | ftxui::underlined;
        WalkChildren(block,state,dec);
        break;
      case ContentBlockType::SPAN:
        WalkChildren(block,state,dec);
        break;
      case ContentBlockType::B:
        dec = dec | ftxui::bold;
        WalkChildren(block,state,dec);
        break;
      case ContentBlockType::CODE:
        WalkChildren(block,state,dec);
        break;
      case ContentBlockType::IMG:
         state.inline_.push_back(ftxui::text("📸") | ftxui::hyperlink(block.url));
        break;
      default:
        state.Flush();
        state.blocks_.push_back(WalkBlock(block) | dec);
        break;
    }
  };

  void PushText(
    std::string_view text,
    WalkState& state,
    ftxui::Decorator dec = ftxui::nothing
  )
  {
    size_t i = 0, n = text.size();
     while (i < n) {
       while (i < n && std::isspace((unsigned char)text[i])) ++i;
       size_t start = i;
       while (i < n && !std::isspace((unsigned char)text[i])) ++i;
       if (start != i) PushWord(text.substr(start, i - start),state,dec);
     }
  }

  void PushWord(
    std::string_view word,
    WalkState& state,
    ftxui::Decorator dec = ftxui::nothing
  )
  {
    state.inline_.push_back(ftxui::text(word) | dec);
    state.inline_.push_back(ftxui::text(" ") | dec);
  }

  void WalkChildren(ContentBlock& block, WalkState& state, ftxui::Decorator dec = ftxui::nothing)
  {
    for(auto& child  : block.children)
    {
      if(IsInNewLine(child.type))
      {
        state.Flush();
        state.blocks_.push_back(WalkBlock(child) | dec);
      }
      else
      {
        WalkInLine(child,state,dec);
      }

    }
    state.Flush();
  }

  ArticleState state_;
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
