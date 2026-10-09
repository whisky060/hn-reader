#ifndef COMMENT_ENTRY_HPP
#define COMMENT_ENTRY_HPP
#include <functional>
#include <string>
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/mouse.hpp>
#include <vector>
#include "model/DataBlock.hpp"
#include "widgets/Spinner/spinner.hpp"


inline ftxui::Component CommentEntry(CommentDataBlock comment, std::function<void(std::vector<ID>)> loadSub) {
    class CommentEntryImpl : public ftxui::ComponentBase {
    public:
        CommentEntryImpl(CommentDataBlock comment, std::function<void(std::vector<ID>)> loadSub) : comment_(comment), loadSub_(loadSub)
        {
            show = false;
            isLoading = false;
            sub_ = ftxui::Container::Vertical({});
            commentEntry = ftxui::Collapsible(comment.text_,sub_,&show);
            spinner_  = Spinner({
              "●     ",
              " ●    ",
              "  ●   ",
              "   ●  ",
              "    ● ",
              "     ●",
              "    ● ",
              "   ●  ",
              "  ●   ",
              " ●    "
            });
            this->Add(commentEntry);
            this->Add(spinner_);
        }

        ftxui::Element OnRender() override
        {
            return this->Render();
        }

        bool OnEvent(ftxui::Event event) override
        {
            // if sub as no elements we need to load subcomments
            if(sub_->ChildCount() && !isLoading)
            {
              sub_->Add(spinner_);
              loadSub_(comment_.comments_);
            }

            if(event.is_mouse() && event.mouse().button == ftxui::Mouse::Left)
            {
              return ftxui::ComponentBase::OnEvent(event);
            }

            if(event == ftxui::Event::Return)
            {
              return ftxui::ComponentBase::OnEvent(event);
            }

            return ftxui::ComponentBase::OnEvent(event);
        }

        bool Focusable() const override
        {
            return true;
        }

    private:
        CommentDataBlock comment_;
        std::function<void(std::vector<ID>)> loadSub_;
        bool show;
        bool isLoading;
        ftxui::Component sub_;
        ftxui::Component commentEntry;
        ftxui::Component spinner_;
    };
    return ftxui::Make<CommentEntryImpl>(comment, loadSub);
}



#endif // COMMENT_ENTRY_HPP
