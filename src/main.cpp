#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <memory>
#include <thread>
#include <iostream>
#include "controller/app_controller.hpp"
#include "model/app_model.hpp" 

int main() {
 
 try {

    App app = App::TerminalOutput();
    auto model = std::make_shared<AppModel>();
    auto controller = ftxui::Make<AppController>(model);
    app.Loop(controller);
    std::thread([&] {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        app.PostEvent(ftxui::Event::Custom);
    }
}).detach();
  } catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}