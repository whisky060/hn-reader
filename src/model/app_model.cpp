#include "app_model.hpp"


AppModel::AppModel() {
   this->news_list = {
      "entry 1",
      "entry 2",
      "entry 3",
      "entry 1",
      "entry 2",
      "entry 3",
      "entry 1",
      "entry 2",
      "entry 3",
      "entry 1",
      "entry 2",
      "entry 3",
      "entry 1",
      "entry 2",
      "entry 3",
    };
    
    this->client = new HNClient();
    client->FetchToStories();
}

AppModel::~AppModel() {

}

