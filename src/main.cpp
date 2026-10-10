#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/loop.hpp>
#include <memory>
#include <thread>
#include <iostream>
#include "controller/app_controller.hpp"
#include "model/app_model.hpp"
#include "concurrency/Dispatcher.hpp"


int main() {
  // https://www.eff.org/deeplinks/2026/10/court-agrees-eff-utahs-vpn-law-demands-technical-impossibility
  // https://www.leadedsolder.com/2026/10/01/rc2026-10-sg1000-forth-port.html
  // need to test
 try {

    App app = App::Fullscreen();
    auto dispatcher = std::make_shared<Dispatcher>();
    auto model = std::make_shared<AppModel>(dispatcher);
    auto controller = ftxui::Make<AppController>(model);
    ftxui::Loop loop(&app, controller);

    while (!loop.HasQuitted())
    {
      dispatcher->Drain();
      loop.RunOnce();
      std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
    }





  } catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
