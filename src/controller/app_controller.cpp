#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>
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
                                            auto articleContent = ArticleView(
                                                [model = model]() { return model->GetArticleContent(); },
                                                [this]() { this->template_->PopView(); }
                                            );

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

	 DrainException();

    // Render the component
    return template_->Render();
}

void AppController::DrainException()
{
	std::exception_ptr e = model_->GetPendingException();
	if(!e)
	{
		return;
	}

	try {
		std::rethrow_exception(e);
	} catch (std::exception& ex) {
		template_->PopView();
		template_->PushAlert(ftxui::Renderer([]{return ftxui::text("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); }));
	}
}

bool AppController::OnEvent(Event event)  {
    // Handle events

    if(ComponentBase::OnEvent(event))
    {
        return true;
    }

    return ftxui::ComponentBase::OnEvent(event);
}
