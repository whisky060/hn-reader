#ifndef APP_MODEL_H
#define APP_MODEL_H

#include <vector>
#include <string>
#include "HNClient.hpp"

struct ShownIdWindow{
    int last_shown_indx = -1;
    std::vector<long> ids;
};

class AppModel {
    public:
        AppModel();
        ~AppModel();


        int selected = 0;
        std::vector<std::string> news_list;

    private:
        HNClient* client = nullptr;
        ShownIdWindow* shown_ids;
};


#endif // APP_MODEL_H