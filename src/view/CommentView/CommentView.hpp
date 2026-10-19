#ifndef COMMENTVIEW_HPP
#define COMMENTVIEW_HPP


#include <exception>
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/mouse.hpp>
#include <functional>
#include <memory>
#include <vector>
#include "concurrency/FetchResult.hpp"
#include "model/DataBlock.hpp"
#include "model/HNClient.hpp"
#include "model/app_model.hpp"
#include "view/BaseView.hpp"
#include "widgets/Spinner/patterns.hpp"
#include "widgets/Spinner/spinner.hpp"
#include "view/CommentView/CommentEntry.hpp"

enum class CommentStatus
{
  Loading,
  Ready
};

inline std::shared_ptr<BaseView> CommentView(
  std::vector<ID> commentIDs,
  std::function<void(std::vector<ID>,CommentCallback)> onLoad,
  std::function<void()> exitCallback
) {
    class CommentViewImpl : public BaseView {
    public:
        CommentViewImpl(
          std::vector<ID> commentIDs,
          std::function<void(std::vector<ID>,CommentCallback)> onLoad,
          std::function<void()> exitCallback) :  exitCallback_(exitCallback), BaseView()
        {
          status_ = CommentStatus::Loading;
          onLoad_ = onLoad;
          comments_ = ftxui::Container::Vertical({});
          spinner_ = Spinner(spinner_patterns::HnLoading, 1000ms);
          auto callback  = [this](FetchResult<std::vector<CommentDataBlock>> res){
            if(res.OK())
            {
              for(auto& comment : res.GetValue())
              {
                this->comments_->Add(CommentEntry(comment, this->onLoad_));
                auto separator = ftxui::Renderer([](){return ftxui::separatorEmpty() | ftxui::notflex ; });
                this->comments_->Add(separator);
              }
              status_ = CommentStatus::Ready;
            }
            else
            {
             std::rethrow_exception(res.GetException());
            }
          };
          this->Add(comments_);
          this->Add(spinner_);
          onLoad_(commentIDs,callback);
        }
        bool OnEvent(ftxui::Event event) override {

          if (event == ftxui::Event::Escape) {
            exitCallback_();
            return true;
          }

          return ftxui::ComponentBase::OnEvent(event);
        }

        std::string GetTitle() const override {
            return "Comments";
        }

        std::vector<Shortcut> GetShortcuts() const override {

            return {{"Escape", "Exit"}};
        }

        ftxui::Element OnRender() override {
            if(status_ == CommentStatus::Loading)
              return spinner_->Render() | ftxui::center;

            return comments_->Render() | ftxui::vscroll_indicator | ftxui::yframe;
        }


    private:

        std::function<void()> exitCallback_;
        std::function<void(std::vector<ID>,CommentCallback)> onLoad_;
        ftxui::Component comments_;
        ftxui::Component spinner_;
        CommentStatus status_;
    };
    return std::shared_ptr<BaseView>(new CommentViewImpl(commentIDs,onLoad,exitCallback));
}


#endif
