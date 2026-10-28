#ifndef CULLNODE_HPP
#define CULLNODE_HPP


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
#include <memory>


inline ftxui::Element CullNode(ftxui::Element content)
{
  class CullNode : public ftxui::Node
  {

    public:

      explicit CullNode(ftxui::Element content) : ftxui::Node({content}){};
      void SetBox(ftxui::Box box) override
      {
        ftxui::Node::SetBox(box);
        children_[0]->SetBox(box);
      }

      void Render(ftxui::Screen& screen) override
			{

           	if(!ftxui::Box::Intersection(box_, screen.stencil).IsEmpty())
             {
             		Node::Render(screen);
             }
            return;
			};
    private:

  };
  auto node = std::make_shared<CullNode>(content);
  return node;
}



#endif
