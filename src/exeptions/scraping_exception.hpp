#ifndef SCRAPING_EXCEPTION_H
#define SCRAPING_EXCEPTION_H

#include <string>
#include <exception>


class ScrapingException : public std::exception
{
private:
    std::string message_;
    std::string url_;
public:
    ScrapingException():message_("Error during website scraping"){};
    ScrapingException(std::string msg) : message_(msg){};
    ScrapingException(std::string msg, std::string url) : message_(msg), url_(url) {};
    const std::string GetUrl() const {return url_; };

    virtual const char* what() const noexcept override {
        return message_.c_str();
    }
};




#endif
