#ifndef DATA_BLOCK_H
#define DATA_BLOCK_H

#include <nlohmann/json.hpp>
#include "HNClient.hpp"
#include <cstdint>

enum class DataBlockType : uint8_t
{
    Story,
    Comment,
    Job
};


// contains all informations about a story
struct StoryDataBlock
{
        DataBlockType type_;
        std::string author_;
        unsigned long descendants_;
        ID id_;
        std::vector<ID> comments_;
        int score_;
        unsigned long time_;
        std::string title_;
        std::string url_;

        StoryDataBlock() = default;
        StoryDataBlock(DataBlockType type,
                        std::string author,
                        unsigned long descendants,
                        ID id,
                        std::vector<ID> comments,
                        int score,
                        unsigned long time,
                        std::string title,
                        std::string url
                        ):
                         type_(type),
                         author_(author),
                         descendants_(descendants),
                         id_(id),
                         score_(score),
                         time_(time),
                         title_(title),
                         url_(url)
        {
                comments_ = std::move(comments);
        }
};

// contains all information about a comment
struct CommentDataBlock
{
        DataBlockType type_;
        std::string author_;
        ID id_;
        std::vector<ID> comments_;
        ID parent_;
        std::string text_;
        unsigned long time_;

        CommentDataBlock() = default;
        CommentDataBlock(DataBlockType type,
                        std::string author,
                        ID id,
                        std::vector<ID> comments,
                        ID parent,
                        std::string text,
                        unsigned long time
                        ):
                        type_(type),
                        author_(author),
                        id_(id),
                        parent_(parent),
                        text_(text),
                        time_(time)
        {
                comments_ = std::move(comments);
        }

};


// contains all information about a job listing
// https://hacker-news.firebaseio.com/v0/jobstories.json
struct JobDataBlock
{
        DataBlockType type_;
        std::string author_;
        ID id_;
        int score_;
        unsigned long time_;
        std::string title_;
        std::string url_;

        JobDataBlock() = default;
        JobDataBlock(DataBlockType type,
                     std::string author,
                     ID id,
                     int score,
                     unsigned long time,
                     std::string title,
                     std::string url
                     ):
                     type_(type),
                     author_(author),
                     id_(id),
                     score_(score),
                     time_(time),
                     title_(title),
                     url_(url)
        {
        }

};

#endif // DATA_BLOCK_H
