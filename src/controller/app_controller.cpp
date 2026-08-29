#include <ftxui/component/component.hpp>
#include "app_controller.hpp"
#include "view/template/Template.hpp"
#include "view/template/BaseTemplate.hpp"
#include "view/ArticleList/ArticleListView.hpp"
#include <vector>
#include <string>

AppController::AppController(std::shared_ptr<AppModel> model): ComponentBase()
{
model_ = model;
    model_->LoadMoreStories();
    template_ = BaseTemplate(ArticleList([model = model_]() { return model->GetArticlesList(); },
                                        [this](StoryDataBlock story)
                                        { /* TODO make the the creation fo the view of the data */},
                                        [this]()
                                        {this->model_->LoadMoreStories();},
                                        [this](){
                                            return this->model_->isLoading();
                                        }
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
 