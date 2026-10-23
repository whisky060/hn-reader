#include <cstddef>
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/loop.hpp>
#include <memory>
#include <thread>
#include <iostream>
#include <image_view.hpp>
#include "controller/app_controller.hpp"
#include "model/app_model.hpp"
#include "concurrency/Dispatcher.hpp"
#include "model/Cache/LruCache.hpp"

int main() {
  // https://www.eff.org/deeplinks/2026/10/court-agrees-eff-utahs-vpn-law-demands-technical-impossibility
  // https://www.leadedsolder.com/2026/10/01/rc2026-10-sg1000-forth-port.html
  // https://zachlandes.com/trolley-problem/
  // https://www.derekthompson.org/p/i-know-why-the-mathematicians-weep no header?
  // https://www.currentaffairs.org/news/why-were-banning-ai no full article
  // https://philzimmermann.com/EN/essays/WhyIWrotePGP.html
  // articles to make tests on
 try {

    App app = App::Fullscreen();
    auto dispatcher = std::make_shared<Dispatcher>();
    auto model = std::make_shared<AppModel>(dispatcher);
    auto controller = ftxui::Make<AppController>(model);
    ftxui::Loop loop(&app, controller);

    ftxui::setOnImageLoadedCallback([&]() {
            app.PostEvent(ftxui::Event::Custom);
    });

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
