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
    
    client = new HNClient();
    shown_ids = new ShownIdWindow(); 
    shown_ids->ids = std::move(client->FetchToStories());
   
}

AppModel::~AppModel() {
   delete client;
   delete shown_ids;
}

