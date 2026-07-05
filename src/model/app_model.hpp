#ifndef APP_MODEL_H
#define APP_MODEL_H

#include <vector>
#include <string>
#include "HNClient.hpp"

class AppModel {
    public:
        AppModel();
        ~AppModel();

        int selected = 0;
        std::vector<std::string> news_list;

    private:
        HNClient* client = nullptr;
};


#endif // APP_MODEL_H