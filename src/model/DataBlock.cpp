#include "DataBlock.hpp"


AbstractDataBlock::AbstractDataBlock(DataBlockType type):
    type(type)
{

}

StoryDataBlock::StoryDataBlock(nlohmann::json json_data):
    AbstractDataBlock(DataBlockType::Story),
    author_(json_data.value("author", "")),
    descendants_(json_data.value("descendants", 0)),
    id_(json_data.value("id", 0)),
    comments_(json_data.value("comments", std::vector<unsigned long>())),
    score_(json_data.value("score", 0)),
    time_(json_data.value("time", 0)),
    title_(json_data.value("title", "")),
    url_(json_data.value("url", ""))
    
{
    
}

StoryDataBlock::~StoryDataBlock(){}

CommentDataBlock::CommentDataBlock(nlohmann::json json_data):
    AbstractDataBlock(DataBlockType::Comment),
    author_(json_data.value("author", "")),
    id_(json_data.value("id", 0)),
    comments_(json_data.value("comments", std::vector<unsigned long>())),
    parent_(json_data.value("parent", 0)),
    text_(json_data.value("text", "")),
    time_(json_data.value("time", 0))
{
}

CommentDataBlock::~CommentDataBlock(){}

JobDataBlock::JobDataBlock(nlohmann::json json_data):
    AbstractDataBlock(DataBlockType::Job),
    author_(json_data.value("author", "")),
    id_(json_data.value("id", 0)),
    score_(json_data.value("score", 0)),
    time_(json_data.value("time", 0)),
    title_(json_data.value("title", "")),
    url_(json_data.value("url", ""))
{
}

JobDataBlock::~JobDataBlock(){}