#ifndef NEWS_LIST_VIEW_H
#define NEWS_LIST_VIEW_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <vector>
#include <string>
#include <memory>
using namespace ftxui;

Component NewsListView(std::shared_ptr<AppModel> model) {
  auto menu = Menu(&model->news_list, &model->selected);  
  return Renderer(menu,[model,menu] {
    return vbox({
      text("News List View"),
      separator(),
      menu->Render(),
    });
  });

}

#endif