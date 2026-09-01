#ifndef SCRAPING_EXCEPTION_H
#define SCRAPING_EXCEPTION_H

#include <string>
#include <exception>


class ScrapingException : public std::exception
{
private:
    std::string message_;
public:
    ScrapingException():message_("Error during website scraping"){};
    ScrapingException(std::string msg) : message_(msg){}


    virtual const char* what() const noexcept override {
        return message_.c_str();
    }
};




#endif