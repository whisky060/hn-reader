#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include <exception>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include "view/template/Template.hpp"
#include "model/app_model.hpp"
#include "view/AelrtView/AlertView.hpp"
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
    Element OnRender() override;
    bool OnEvent(Event event) override;

private:
    void FetchArticles(){ model_->FetchTopStories(); };
    void HandleException(std::exception_ptr exc);
    Component curr_view;
    std::shared_ptr<AppModel> model_;
    std::shared_ptr<AbstractTemplate> template_;
    std::shared_ptr<bool> alive_ = std::make_shared<bool>(true);

};


#endif // APP_CONTROLLER_H
