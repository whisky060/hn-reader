#ifndef APP_MODEL_H
#define APP_MODEL_H

#include <vector>
#include <string>
#include "HNClient.hpp"

// Class to hold the state of the shown IDs window
class ShownIdWindow{
    public:
        ShownIdWindow() = default;
        ShownIdWindow(std::vector<long> id_list){
            ids = std::move(id_list);
        }
        ~ShownIdWindow() = default;
        int GetLastShownIndex() const { return LastShownIndex; }
        bool HasNext() const { return GetLastShownIndex() < ids.size();}
        void RefreshWindow(std::vector<long> new_ids) {
            LastShownIndex = -1;
            ids = std::move(new_ids);
        }

    private:
        int LastShownIndex = -1;
        std::vector<long> ids;
};


class AppModel {
    public:
        AppModel();
        ~AppModel();

        std::vector<long> FetchTopStories();
        void FetchStory(long id);

        int selected = 0;
        std::vector<std::string> news_list;

    private:
        
        HNClient* client = nullptr;
        ShownIdWindow* shown_ids;
};


#endif // APP_MODEL_H