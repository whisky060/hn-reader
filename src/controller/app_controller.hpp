#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include "model/app_model.hpp"
#include <memory>

/*
    the controller is responsible for handling usr input.
    it will receive events from the view and update the model and the view accordingly.
    it will also be responsible for the navigation between different views.
*/


using namespace ftxui;


class AppController : public ComponentBase {
public:
    AppController(std::shared_ptr<AppModel> model);
    ~AppController();
    Element OnRender();
    bool OnEvent(Event event);

private:
    Component curr_view;
    std::shared_ptr<AppModel> model;

};


#endif // APP_CONTROLLER_H