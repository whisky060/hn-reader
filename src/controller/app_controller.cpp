#include <ftxui/component/component.hpp>
#include "app_controller.hpp"
#include "view/news_list_view.hpp"
#include "view/Template.hpp"
#include <vector>
#include <string>

AppController::AppController(std::shared_ptr<AppModel> model) :
ComponentBase(),
model_(model),
template_(BaseTemplate(Renderer([]{return text("asdas");})))
{
    curr_view = NewsListView(model_);
    this->Add(curr_view);  
    
    this->FetchArticles();
}

AppController::~AppController() {

}

Element AppController::OnRender()  {
    // Render the component
    return template_->Render();
}

bool AppController::OnEvent(Event event) {
    // Handle events 

    
    return template_->OnEvent(event); 
}
 