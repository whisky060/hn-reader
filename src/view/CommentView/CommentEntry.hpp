#ifndef COMMENT_ENTRY_HPP
#define COMMENT_ENTRY_HPP

#include <functional>
#include <memory>
#include <stdexcept>
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/mouse.hpp>
#include <string>
#include <vector>
#include "concurrency/FetchResult.hpp"
#include "model/DataBlock.hpp"
#include "model/app_model.hpp"
#include "view/ContentBlockRenderer/ContentBlockRenderer.hpp"
#include "widgets/Spinner/spinner.hpp"
#include "widgets/Spinner/patterns.hpp"
#include "widgets/WrapCollapsible/WrapCollapsible.hpp"

enum class CommentEntryState{
  READY,
  LOADING,
  ERROR
};

inline ftxui::Component CommentEntry(CommentDataBlock comment, std::function<void(std::vector<ID>, CommentCallback)> loadSub) {
    class CommentEntryImpl : public ftxui::ComponentBase {
    public:
        CommentEntryImpl(CommentDataBlock comment, std::function<void(std::vector<ID>, CommentCallback)> loadSub) : comment_(comment), loadSub_(loadSub)
        {
            state_ = CommentEntryState::LOADING;
            show = false;
            isLoading = false;
            sub_ = ftxui::Container::Vertical({});
            commentEntry = WrapCollapsible(ContentBlockRenderer::ContentComponent(comment.root_),sub_,&show);
            spinner_ = Spinner(spinner_patterns::Dots, 1000ms);
            callback = [this, alive = alive_](FetchResult<std::vector<CommentDataBlock>> res)
              {
                if(!*alive)
                  return;
                if(res.OK())
                {
                  sub_->DetachAllChildren();

                  for(auto& comment : res.GetValue())
                  {
                    this->sub_->Add(CommentEntry(comment, this->loadSub_));
                    auto separator = ftxui::Renderer([](){return ftxui::separatorEmpty() | ftxui::notflex ; });
                    this->sub_->Add(separator);
                  }
                  state_ = CommentEntryState::READY;
                  return;
                }
                state_ = CommentEntryState::ERROR;
                throw std::runtime_error("error in laoding the commets");
              };
            this->Add(commentEntry);

        }

        ~CommentEntryImpl() override { *alive_ = false; }

        ftxui::Element OnRender() override
        {
            return ftxui::vbox({
              ftxui::hbox({
                ftxui::text(comment_.author_) | ftxui::notflex,
                ftxui::text(" - ") | ftxui::notflex,
                ftxui::text("comments(" + std::to_string(comment_.comments_.size()) + ")"),
              }),
              ftxui::separatorCharacter("-") | ftxui::notflex,
              commentEntry->Render()
            });
        }

        bool OnEvent(ftxui::Event event) override
        {


            if(event.is_mouse() && event.mouse().button == ftxui::Mouse::Left && !comment_.comments_.empty())
            {
              loadSub_(comment_.comments_, callback);
              if(sub_->ChildCount() <= 0)
                sub_->Add(spinner_);
              return ftxui::ComponentBase::OnEvent(event);
            }

            if(event == ftxui::Event::Return && !comment_.comments_.empty())
            {
              loadSub_(comment_.comments_, callback);
              if(sub_->ChildCount() <= 0)
                sub_->Add(spinner_);
              return ftxui::ComponentBase::OnEvent(event);
            }

            return ftxui::ComponentBase::OnEvent(event);
        }

        bool Focusable() const override
        {
            return true;
        }

    private:
        CommentCallback callback;
        CommentDataBlock comment_;
        std::function<void(std::vector<ID>, CommentCallback)> loadSub_;
        bool show;
        bool isLoading;
        ftxui::Component sub_;
        ftxui::Component commentEntry;
        ftxui::Component spinner_;
        CommentEntryState state_;
        std::shared_ptr<bool> alive_ = std::make_shared<bool>(true);
    };
    return ftxui::Make<CommentEntryImpl>(comment, loadSub);
}



#endif // COMMENT_ENTRY_HPP
