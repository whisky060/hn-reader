#ifndef HTML_PARSING_ERROR
#define HTML_PARSING_ERROR

#include <string>
#include <exception>

class HTMLParsingException : public std::exception
{
private:
    std::string message_;
public:
    HTMLParsingException() : message_("an error occurred during the parsing of the article html page"){}
    HTMLParsingException(std::string msg) : message_(msg){};

    virtual const char* what() const noexcept override {
        return message_.c_str();
    }
};



#endif