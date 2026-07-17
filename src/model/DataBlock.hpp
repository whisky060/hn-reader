#ifndef DATA_BLOCK_H
#define DATA_BLOCK_H

#include <nlohmann/json.hpp>
#include <cstdint>

enum class DataBlockType : uint8_t
{
    Story,
    Comment,
    Job
};


/*
    Abstract base class for all data blocks
*/
class AbstractDataBlock
{
    public:
        AbstractDataBlock(DataBlockType type);
        virtual ~AbstractDataBlock() = default;
        DataBlockType getType() const { return type; }
    private:
        DataBlockType type;


};

// contains all informations about a story
class StoryDataBlock : public AbstractDataBlock
{
    public:
        StoryDataBlock(nlohmann::json json_data);
        ~StoryDataBlock();
        const std::string& getAuthor() const { return author_; }
        unsigned long getDescendants() const { return descendants_; }
        unsigned long getId() const { return id_; }
        const std::vector<unsigned long>& getComments() const { return comments_; }
        int getScore() const {return score_;}
        unsigned long getTime() const { return time_; }
        const std::string& getTitle() const { return title_; }
        const std::string& getUrl() const { return url_; }
    private:
        std::string author_;
        unsigned long descendants_;
        unsigned long id_;
        std::vector<unsigned long> comments_;
        int score_;
        unsigned long time_;
        std::string title_;
        std::string url_;

};

// contains all information about a comment
class CommentDataBlock : public AbstractDataBlock
{
    public:
        CommentDataBlock(nlohmann::json json_data);
        ~CommentDataBlock();
        const std::string& getAuthor() const { return author_; }
        unsigned long getId() const { return id_; }
        const std::vector<unsigned long>& getComments() const { return comments_; }
        unsigned long getParent() const { return parent_; }
        const std::string& getText() const { return text_; }
    private:
        std::string author_;
        unsigned long id_;
        std::vector<unsigned long> comments_;
        unsigned long parent_;
        std::string text_;
        unsigned long time_;
};


// contains all information about a job listing
// https://hacker-news.firebaseio.com/v0/jobstories.json
class JobDataBlock : public AbstractDataBlock 
{
    public:
        JobDataBlock(nlohmann::json json_data);
        ~JobDataBlock();
        const std::string& getAuthor() const { return author_; }
        unsigned long getId() const { return id_; }
        int getScore() const { return score_; }
        unsigned long getTime() const { return time_; }
        const std::string& getTitle() const { return title_; }
        const std::string& getUrl() const { return url_; } 
    private:
        std::string author_;
        unsigned long id_;
        int score_;
        unsigned long time_;
        std::string title_;
        std::string url_;
};












#endif // DATA_BLOCK_H