#ifndef SCROLLVIEW_HPP
#define SCROLLVIEW_HPP

/*
 *  This code is hugely based on the https://github.com/zvasilev/MarkdownFTXUI implementation
*/

#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/box.hpp>
#include <ftxui/screen/screen.hpp>
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


inline ftxui::Component ScrollView(std::function<ftxui::Element()> contentFactory)
{

	struct ScrollState
	{
		float ratio = 0.0;
		int viewPortHeight = 0;
		int contentHeight = 0;
	};

	class ScrollFrame : public ftxui::Node
	{
		public:
			ScrollFrame(ftxui::Element content, std::shared_ptr<ScrollState> state) : ftxui::Node({content}), state_(state) {}

			void SetContent(ftxui::Element content)
			{
				if (content != children_[0])
					children_[0] = std::move(content);
			}

			void ComputeRequirement() override
			{
				children_[0]->ComputeRequirement();
				requirement_ = children_[0]->requirement();
			};

			void SetBox(ftxui::Box box) override
			{
				Node::SetBox(box);
			 	int viewport = box.y_max - box.y_min;
        		int content = std::max(children_[0]->requirement().min_y, viewport);

          		state_->viewPortHeight = viewport;
            	state_->contentHeight = content;

             	int scrollable = std::max(0, content - viewport - 1);
              	int dy = static_cast<int>(state_->ratio * float(scrollable));
               	dy = std::clamp(dy, 0, scrollable);

                ftxui::Box child_box = box;
                child_box.y_min = box.y_min - dy;
                child_box.y_max = box.y_min + content - dy;

                children_[0]->SetBox(child_box);
			};

			void Render(ftxui::Screen& screen) override
			{
				auto stencil = screen.stencil;
        		screen.stencil = ftxui::Box::Intersection(box_, screen.stencil);
          		children_[0]->Render(screen);
            	screen.stencil = stencil;
			};

		private:
			std::shared_ptr<ScrollState> state_;

	};

	std::shared_ptr<ScrollState> state = std::make_shared<ScrollState>();
	ftxui::Element frame = std::make_shared<ScrollFrame>(contentFactory(), state);
	auto component = ftxui::CatchEvent(
    ftxui::Renderer([frame, contentFactory] {
        	auto scrollFrame = std::static_pointer_cast<ScrollFrame>(frame);
        	scrollFrame->SetContent(contentFactory());
        	return frame;
    	}),
    	[state](ftxui::Event ev) {


             // easiest: get state from a static or capture
             auto update = [&](float delta) {

                 auto& ratio = state->ratio;
                 ratio = std::clamp(ratio + delta, 0.0f, 1.0f);
                 return true;
             };

             if (ev == ftxui::Event::ArrowDown) return update(0.03f);
             if (ev == ftxui::Event::k) return update(0.03f);
             if (ev == ftxui::Event::ArrowUp)   return update(-0.03f);
             if (ev == ftxui::Event::j) return update(-0.03f);
             if (ev == ftxui::Event::PageDown)  return update(0.30f);
             if (ev == ftxui::Event::PageUp)    return update(-0.30f);
             if (ev == ftxui::Event::Home) {
                 state->ratio = 0.0f;
                 return true;
             }
             if (ev == ftxui::Event::End) {
                 state->ratio = 1.0f;
                 return true;
             }

             // wheel
             if (ev.is_mouse()) {
                 auto m = ev.mouse();
                 if (m.button == ftxui::Mouse::WheelUp)   return update(-0.005f);
                 if (m.button == ftxui::Mouse::WheelDown) return update(0.005f);
             }

             return false;
     	}
	);
	return component;

}













#endif // SCROLLVIEW_HPP
