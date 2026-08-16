#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <thread>
#include <iostream>
#include "controller/app_controller.hpp"
#include "model/app_model.hpp" 

int main() {
  using namespace ftxui;
 try {

    App app = App::Fullscreen();
    auto model = std::make_shared<AppModel>();
    auto controller = Make<AppController>(model);
    app.Loop(controller);
  } catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}