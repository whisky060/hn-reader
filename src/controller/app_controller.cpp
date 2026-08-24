#include <ftxui/component/component.hpp>
#include "app_controller.hpp"
#include "view/Template.hpp"
#include "view/ArticleListView.hpp"
#include <vector>
#include <string>

AppController::AppController(std::shared_ptr<AppModel> model): ComponentBase()
{
    model_ = model;
    template_ = BaseTemplate(ArticleList(model_->GetArticlesList(),
                                        [this](StoryDataBlock story)
                                        { /* TODO make the the creation fo the view of the data */},
                                        [this]()
                                        {this->model_->LoadMoreStories();}
                            ));
    this->Add(template_);
}

AppController::~AppController() {

}

Element AppController::OnRender() {
    // Render the component
    return template_->Render();
}

bool AppController::OnEvent(Event event)  {
    // Handle events    
    
    
        if(ComponentBase::OnEvent(event))
        {
            return true;
        }
    
    return false; 
}
 