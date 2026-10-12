#ifndef COMMENT_ENTRY_HPP
#define COMMENT_ENTRY_HPP

#include <functional>
#include <stdexcept>
#include <string>
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/mouse.hpp>
#include <vector>
#include "concurrency/FetchResult.hpp"
#include "model/DataBlock.hpp"
#include "model/app_model.hpp"
#include "widgets/Spinner/spinner.hpp"
#include "widgets/Spinner/patterns.hpp"
#include "widgets/WrapCollapsible/WrapCollapsible.hpp"


inline ftxui::Component CommentEntry(CommentDataBlock comment, std::function<void(std::vector<ID>, CommentCallback)> loadSub) {
    class CommentEntryImpl : public ftxui::ComponentBase {
    public:
        CommentEntryImpl(CommentDataBlock comment, std::function<void(std::vector<ID>, CommentCallback)> loadSub) : comment_(comment), loadSub_(loadSub)
        {
            show = false;
            isLoading = false;
            sub_ = ftxui::Container::Vertical({});
            commentEntry = WrapCollapsible(comment.text_,sub_,&show);
            spinner_ = Spinner(spinner_patterns::Dots);
            callback = [this](FetchResult<std::vector<CommentDataBlock>> res)
              {
                if(res.OK())
                {
                  sub_->DetachAllChildren();

                  for(auto& comment : res.GetValue())
                  {
                    this->sub_->Add(CommentEntry(comment, this->loadSub_));
                    auto separator = ftxui::Renderer([](){return ftxui::separatorEmpty() | ftxui::notflex ; });
                    this->sub_->Add(separator);
                  }
                  return;
                }

                throw std::runtime_error("error in laoding the commets");
              };
            this->Add(commentEntry);

        }

        ftxui::Element OnRender() override
        {
            return commentEntry->Render();
        }

        bool OnEvent(ftxui::Event event) override
        {


            if(event.is_mouse() && event.mouse().button == ftxui::Mouse::Left)
            {
              loadSub_(comment_.comments_, callback);
              if(sub_->ChildCount() <= 0)
                sub_->Add(spinner_);
              return ftxui::ComponentBase::OnEvent(event);
            }

            if(event == ftxui::Event::Return)
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
    };
    return ftxui::Make<CommentEntryImpl>(comment, loadSub);
}



#endif // COMMENT_ENTRY_HPP
