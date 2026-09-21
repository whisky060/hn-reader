#ifndef CONTENT_BLOCK_HPP
#define CONTENT_BLOCK_HPP
#include <string>
#include <sys/types.h>
#include <vector>


enum ContentBlockType {
    H1,
    H2,
    H3,
    P,
    TEXT,
    A,
    TABLE,
    TR,
    TD,
    PRE,
    B,
    I,
    U,
    BR,
    HR,
    DIV,
    BLOCKQUOTE,
    CODE,
    IMG,
    LI,
    UL,
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
