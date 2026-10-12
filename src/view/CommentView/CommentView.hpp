#ifndef COMMENTVIEW_HPP
#define COMMENTVIEW_HPP


#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/mouse.hpp>
#include <functional>
#include <memory>
#include "view/BaseView.hpp"

inline std::shared_ptr<BaseView> CommentView(
  ftxui::Components comments,
  std::function<void()> exitCallback
) {
    class CommentViewImpl : public BaseView {
    public:
        CommentViewImpl(ftxui::Components comments, std::function<void()> exitCallback) :  exitCallback_(exitCallback), BaseView()
        {
          comments_ = ftxui::Container::Vertical(comments);
          this->Add(comments_);
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
            return comments_->Render();
        }

    private:
        std::function<void()> exitCallback_;
        ftxui::Component comments_;

    };
    return std::shared_ptr<BaseView>(new CommentViewImpl(comments,exitCallback));
}


#endif
