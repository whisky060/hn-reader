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
#include "model/DataBlock.hpp"
#include <vector>

inline ftxui::Component CommentEntry(CommentDataBlock comment, std::function<ftxui::Component(std::vector<ID>)> loadSub) {
    class CommentEntryImpl : public ftxui::ComponentBase {
    public:
        CommentEntryImpl(CommentDataBlock comment, std::function<ftxui::Component(std::vector<ID>)> loadSub) : comment_(comment), loadSub_(loadSub)
        {
            show = false;
            commentEntry = ftxui::Collapsible(comment.text_,sub_,show);
            this->Add(commentEntry);
        }

        ftxui::Element OnRender() override
        {
            return this->Render();
        }

        bool OnEvent(ftxui::Event event) override
        {
            if(event.is_mouse() && event.mouse().button == ftxui::Mouse::Left)
            {
                if(sub_ == nullptr)
                  sub_ = loadSub_(comment_.comments_);
                return ftxui::ComponentBase::OnEvent(event);
            }

            if(event == ftxui::Event::Return)
            {
                if(sub_ == nullptr)
                  sub_ = loadSub_(comment_.comments_);
                return ftxui::ComponentBase::OnEvent(event);
            }

            return false;
        }

        bool Focusable() const override
        {
            return true;
        }

    private:
        CommentDataBlock comment_;
        std::function<ftxui::Component(std::vector<ID>)> loadSub_;
        bool show;
        ftxui::Component sub_;
        ftxui::Component commentEntry;
    };
    return ftxui::Make<CommentEntryImpl>(comment, loadSub);
}



#endif // COMMENT_ENTRY_HPP
