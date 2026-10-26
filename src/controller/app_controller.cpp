#include "app_controller.hpp"
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>
#include <vector>
#include "model/app_model.hpp"
#include "view/MenuView/MenuView.hpp"
#include "view/template/BaseTemplate.hpp"
#include "view/ArticleList/ArticleListView.hpp"
#include "view/ArticleView/ArticleView.hpp"
#include "view/CommentView/CommentView.hpp"
#include "utility/utility.h"
#include "exeptions/scraping_exception.hpp"
#include "exeptions/connection_exception.hpp"

AppController::AppController(std::shared_ptr<AppModel> model): ComponentBase()
{
  model_ = model;
  template_ = BaseTemplate(ArticleList(
                                        [this](std::vector<ID> comments)
                                        {
                                          auto commentView = CommentView(
                                            comments,
                                            [model= model_,this](std::vector<ID> comments_, CommentCallback callback){
                                              model->FetchComments(comments_,callback);
                                            },
                                            [this]() { template_->PopView(); }
                                          );
                                          this->template_->ChangeView(commentView);
                                        },
                                        [this, model = model_](std::string articleUrl)
                                        {
                                          auto articleView = ArticleView(
                                                     // request (higher-order): view calls this ONCE in its ctor, passing its handler
                                                     [model, articleUrl, this](ArticleCallback inner)
                                                     {
                                                         model->FetchArticle(articleUrl,
                                                             // error-interception wrapper: same convention as on_load
                                                             [this, inner, alive = alive_](auto res)
                                                             {
                                                                 if(!*alive)
                                                                     return;
                                                                 try   { inner(res); }
                                                                 catch (...) { HandleException(std::current_exception()); }
                                                             });
                                                     },
                                                     // exit: Esc → back to the list
                                                     [this]() { template_->PopView(); },
                                                     articleUrl
                                                 );

                                            this->template_->ChangeView(articleView);

                                        },
                                        [this, model = model_](ListCallback inner)
                                        {
                                          model->FetchMoreStories([this, inner, alive = alive_](FetchResult<std::vector<StoryDataBlock>> res) {
                                                 if(!*alive)
                                                     return;
                                                 try   { inner(res); }
                                                 catch (...) { HandleException(std::current_exception()); }  // → alert
                                             });
                                        }));
  this->Add(template_);

}

AppController::~AppController() { *alive_ = false; }

Element AppController::OnRender() {

  // Render the component
  return template_->Render();
}

void AppController::HandleException(std::exception_ptr exc)
{

	if(!exc)
	{
		return;
	}

	try {
		std::rethrow_exception(exc);
	}
	catch (ScrapingException& ex)
	{
		template_->PopView();
		template_->PushAlert(
		  AlertView(
				"Error during article loading.\n Do you want to open the article on the browser?",
		    [template_ = template_]() { template_->CloseAlert(); },
		    [template_ = template_, ex]() {
						    utils::OpenUrl(ex.GetUrl());
								template_->CloseAlert();
						}
		  ));
	}

}

void AppController::OpenMenu()
{
  template_->PushAlert(
    MenuView([template_ = template_](){ template_->CloseAlert(); })
  );
}

bool AppController::OnEvent(Event event)  {

    // Handle events
    if(event == Event::CtrlT)
    {
      OpenMenu();
      return true;
    }


    if(ComponentBase::OnEvent(event))
    {
        return true;
    }

    return ftxui::ComponentBase::OnEvent(event);
}
