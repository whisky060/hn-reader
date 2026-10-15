#ifndef ARTICLEENTRY_H
#define ARTICLEENTRY_H
#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/mouse.hpp>
#include <format>
#include <functional>
#include <vector>
#include "utility/utility.h"
#include "model/DataBlock.hpp"

class ArticleEntry : public ftxui::ComponentBase
{
public:
    ArticleEntry(
      StoryDataBlock article,
      std::function<void(std::string)> on_select,
      std::function<void(std::vector<ID>)> on_openComments
    ) :
     article_(article),
     on_select_(on_select),
     on_openComments_(on_openComments)
     {};

    ftxui::Element OnRender() override
    {
        auto content = ftxui::vbox({ftxui::text(article_.title_) | ftxui::notflex,
        							ftxui::separator(),
                                    ftxui::hbox({ftxui::text(article_.author_) | ftxui::notflex,
                                                 ftxui::text(" - ") | ftxui::notflex,
                                                 ftxui::text(utils::GetDateFromUnix(article_.time_)) | ftxui::notflex,
                                                 ftxui::text(" - ") | ftxui::notflex,
                                                 ftxui::text(std::format("Score: {}", article_.score_)) | ftxui::notflex,
                                                 ftxui::text(" - ") | ftxui::notflex,
                                                 ftxui::text(std::format("Url: {}", article_.url_)) | ftxui::notflex,  //| ftxui::hyperlink(article_.url_),
                                                 ftxui::text(" - "),
                                                 ftxui::text(std::format("Comments({})", article_.descendants_)) | ftxui::notflex}),

                                }) | ftxui::notflex;

        // Add visual cues when highlighted by keyboard navigation
        if (Focused())
        {
            content = content | ftxui::inverted | ftxui::focus;
        }
        return content | reflect(box_);
    }

    bool OnEvent(ftxui::Event event) override
    {


    	// fired when enter is pressed
        if(event == ftxui::Event::Return)
        {
            on_select_(this->article_.url_);
            return true;
        }


        if(event == ftxui::Event::CtrlO) {
           utils::OpenUrl(article_.url_);
           return true;
        }

        // opens the comments of the related article
        if(event == ftxui::Event::c)
        {
          on_openComments_(article_.comments_);
          return true;
        }

        if(event.is_mouse())
        {
            if(event.mouse().button == ftxui::Mouse::Left && event.mouse().motion == ftxui::Mouse::Pressed)
            {
                if(box_.Contain(event.mouse().x, event.mouse().y))
                {
                    TakeFocus();
                    return true;
                }
            }
        }

       	if(!Focused())
       	{
       		return ftxui::ComponentBase::OnEvent(event);
       	}



        return ftxui::ComponentBase::OnEvent(event);
    }

    bool  Focusable() const override
    {
        return true;
    }

private:
    std::function<void(std::string)> on_select_;
    std::function<void(std::vector<ID>)> on_openComments_;
    ftxui::Box box_;
    StoryDataBlock article_;
};



#endif // ARTICLEENTRY_H
