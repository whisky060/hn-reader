#ifndef COMMENT_ENTRY_HPP
#define COMMENT_ENTRY_HPP

#include <ftxui/component/app.hpp>
#include <ftxui/screen/box.hpp>
#include <cstddef>
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
#include "model/DataBlock/DataBlock.hpp"
#include "model/app_model.hpp"
#include "theme/themed.hpp"
#include "view/ContentBlockRenderer/ContentBlockRenderer.hpp"
#include "widgets/Spinner/spinner.hpp"
#include "widgets/Spinner/patterns.hpp"
#include "widgets/WrapCollapsible/WrapCollapsible.hpp"

enum class CommentEntryState{
  READY,
  LOADING,
  LOADED,
  FAILED
};

inline ftxui::Component CommentEntry(CommentDataBlock comment, std::function<void(std::vector<ID>, CommentCallback)> loadSub) {
    class CommentEntryImpl : public ftxui::ComponentBase {
    public:
        CommentEntryImpl(CommentDataBlock comment, std::function<void(std::vector<ID>, CommentCallback)> loadSub) : comment_(comment), loadSub_(loadSub)
        {
            state_ = CommentEntryState::READY;
            show = false;
            isLoading = false;
            replyCount_ = comment_.comments_.size();
            sub_ = ftxui::Container::Vertical({});
            commentEntry = WrapCollapsible(ContentBlockRenderer::ContentComponent(comment.root_),sub_,&show);
            spinner_ = Spinner(spinner_patterns::Dots, 1000ms);
            callback = [this, alive = alive_](FetchResult<std::vector<CommentDataBlock>> res)
              {
                if(!*alive)
                  return;
                if(res.OK())
                {
                  auto comments = res.GetValue();
                  sub_->DetachAllChildren();
                  ftxui::Component firstChild = nullptr;
                  for(auto& comment : comments)
                  {

                    auto separator = ftxui::Renderer([](){return ftxui::separatorEmpty() | ftxui::notflex ; });
                    this->sub_->Add(separator);
                    auto entry = CommentEntry(comment, this->loadSub_);
                    this->sub_->Add(entry);
                    if(!firstChild) firstChild = entry;
                    separator =  ftxui::Renderer([](){return ftxui::separatorEmpty() | ftxui::notflex ; });
                    this->sub_->Add(separator);
                  }
                  replyCount_ = comments.size();
                  state_ = CommentEntryState::LOADED;
                  if(replyCount_ == 0)
                      show = false;
                  sub_->SetActiveChild(firstChild);
                  return;
                }
                state_ = CommentEntryState::FAILED;
                throw std::runtime_error("error in laoding the commets");
              };
            this->Add(commentEntry);

        }

        ~CommentEntryImpl() override { *alive_ = false; }

        ftxui::Element OnRender() override
        {
            auto content = ftxui::vbox({
              ftxui::hbox({
                themed::Text(comment_.author_) | ftxui::notflex,
                themed::Text(" - ") | ftxui::notflex,
                themed::Text("comments(" + std::to_string(replyCount_) + ")"),
              }),
              themed::SeparatorCharacter("-") | ftxui::notflex,
              commentEntry->Render()
            }) | ftxui::reflect(box_);



            return content;
        }

        bool OnEvent(ftxui::Event event) override
        {

          if(event.is_mouse())
          {

            if(event.mouse().button == ftxui::Mouse::Left && event.mouse().motion == ftxui::Mouse::Pressed)
            {
                if(box_.Contain(event.mouse().x, event.mouse().y))
                {
                    TakeFocus();
                    LoadComments();
                    if(replyCount_ == 0)
                      return true;

                    return ftxui::ComponentBase::OnEvent(event);
                }
            }
          }

          if(event == ftxui::Event::Return)
          {
            LoadComments();
            if(replyCount_ == 0)
              return true;
            return ftxui::ComponentBase::OnEvent(event);
          }

          return ftxui::ComponentBase::OnEvent(event);
        }

        void LoadComments()
        {
          if( state_ == CommentEntryState::LOADED ||
              state_ == CommentEntryState::LOADING ||
              show ||
              replyCount_ == 0
          )
          {
            return;
          }

          sub_->Add(spinner_);
          state_ = CommentEntryState::LOADING;
          loadSub_(comment_.comments_, callback);
        }

        bool Focusable() const override
        {
            return true;
        }

    private:
        CommentCallback callback;
        CommentDataBlock comment_;
        std::size_t replyCount_ = 0;
        std::function<void(std::vector<ID>, CommentCallback)> loadSub_;
        bool show;
        bool isLoading;
        ftxui::Component sub_;
        ftxui::Component commentEntry;
        ftxui::Component spinner_;
        CommentEntryState state_;
        ftxui::Box box_;
        std::shared_ptr<bool> alive_ = std::make_shared<bool>(true);
    };
    return ftxui::Make<CommentEntryImpl>(comment, loadSub);
}



#endif // COMMENT_ENTRY_HPP
