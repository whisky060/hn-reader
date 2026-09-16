#ifndef CONTENT_BLOCK_HPP
#define CONTENT_BLOCK_HPP
#include <string>
#include <vector>


enum ContentBlockType {
    H1,
    H2,
    H3,
    P,
    TEXT,
    A,
    TABLE,
    TABLEROW,
    PRE,
    B,
    I,
    EM,
    BLOCKQUOTE,
    CODE,
    IMG,
    LI,
    CAPTION
};

struct ContentBlock
{

    ContentBlockType type;
    std::string text;
    std::string url;
    std::vector<ContentBlock> children;

};

#endif // CONTENT_BLOCK_HPP
