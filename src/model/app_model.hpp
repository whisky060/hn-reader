#ifndef APP_MODEL_H
#define APP_MODEL_H

#include <vector>
#include <string>


class AppModel {
    public:
        AppModel();
        ~AppModel();

        int selected = 0;
        std::vector<std::string> news_list;

    private:
        // Add private member variables and methods here
};


#endif // APP_MODEL_H