#include <ftxui/component/component.hpp>
#include "app_controller.hpp"
#include "view/news_list_view.hpp"
#include <vector>
#include <string>

AppController::AppController(std::shared_ptr<AppModel> model) : ComponentBase(), model(model) {
    curr_view = NewsListView(model);
    this->Add(curr_view);    
}

AppController::~AppController() {

}

Element AppController::OnRender()  {
    // Render the component
    return curr_view->Render();
}

bool AppController::OnEvent(Event event) {
    // Handle events 

    
    return curr_view->OnEvent(event); 
}
 