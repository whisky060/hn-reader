#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <memory>
#include <string>
#include <thread>
#include "app_controller.hpp"
#include "view/template/BaseTemplate.hpp"
#include "view/ArticleList/ArticleListView.hpp"
#include "view/ArticleView/ArticleView.hpp"

AppController::AppController(std::shared_ptr<AppModel> model): ComponentBase()
{
model_ = model;
    model_->LoadMoreStories();
    template_ = BaseTemplate(ArticleList([model = model_]() { return model->GetArticlesList(); },
                                        [this, model = model_](std::string articleUrl)
                                        {
                                            model->GetArticle(articleUrl);
                                            auto articleContent = ArticleView([model = model]() { return model->GetArticleContent(); });
                                            this->template_->ChangeView(articleContent);
                                        },
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
