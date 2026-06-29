#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include "controller/app_controller.hpp"
#include "model/app_model.hpp" 

int main() {
  using namespace ftxui;
  App app = App::FullscreenPrimaryScreen();
  auto model = std::make_shared<AppModel>();
  auto controller = Make<AppController>(model);
  app.Loop(controller);
  
  return EXIT_SUCCESS;
}