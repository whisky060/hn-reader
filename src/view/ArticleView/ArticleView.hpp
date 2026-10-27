#ifndef ARTICLEVIEW
#define ARTICLEVIEW

#include "concurrency/FetchResult.hpp"
#include "ftxui/dom/elements.hpp"
#include "model/ContentBlock/ContentBlock.hpp"
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
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "utility/utility.h"
#include "view/BaseView.hpp"
#include "view/ContentBlockRenderer/ContentBlockRenderer.hpp"

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
    state_ = ArticleState::Loading;

    loadingSpinner_ = Spinner(spinner_patterns::HnLoading, 1000ms);
    this->Add(loadingSpinner_);

    request([this, alive = alive_](FetchResult<ContentBlock> res){
        if(!*alive)
          return;
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
  ~ArticleViewImp() override { *alive_ = false; }

  std::string GetTitle() const override {
    return articleUrl_.empty() ? "Article" : articleUrl_;
  }

  std::vector<Shortcut> GetShortcuts() const override {
    return {
        {"Esc", "back"},
        {"Ctrl+O", "browser"},
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
  void CreateArticleContent(const ContentBlock& content) {
    articleBlock_ = std::make_shared<ContentBlock>(content);
    articleContent_ = ScrollView(ContentBlockRenderer::CachedRender(articleBlock_));
    this->Add(articleContent_);
  };

  ArticleState state_;
  std::function<void()> exitCallback_;
  ftxui::Component loadingSpinner_;
  ftxui::Component articleContent_;
  std::shared_ptr<ContentBlock> articleBlock_;
  std::string articleUrl_;
  std::shared_ptr<bool> alive_ = std::make_shared<bool>(true);
};

inline std::shared_ptr<BaseView>
ArticleView(std::function<void(ArticleCallback)> request,
            std::function<void()> exitCallback,
            std::string articleUrl) {
  return std::static_pointer_cast<BaseView>(
      ftxui::Make<ArticleViewImp>(request, exitCallback, articleUrl));
}

#endif
