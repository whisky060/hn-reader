#include "model/ArticleScraper.hpp"


std::string ArticleScraper::operator()(std::string html_page)
{   
    ParseHTML(html_page);
    FreeResources();
    return html_page;
}

void ArticleScraper::ParseHTML(std::string html_page)
{
    doc = lxb_html_document_create();
    if(doc == nullptr){
        FreeResources();
        throw HTMLParsingException("couldn't allocate enough space during parsing creation");
    }
    
    auto code = lxb_html_document_parse(doc, (const lxb_char_t*)html_page.data(), html_page.size());
    if(code != LXB_STATUS_OK)
    {
        FreeResources();
        throw HTMLParsingException("couldn't parse the html page");
    }
    
    dom  = lxb_dom_interface_node(lxb_html_document_body_element(doc));
    
}  

void ArticleScraper::FreeResources()
{   
    if (doc != nullptr)
        lxb_html_document_destroy(doc);
    
    if(dom !=nullptr)
        lxb_dom_node_destroy(dom);
}

void ArticleScraper::ExtractContent()
{
    
}

ArticleScraper::~ArticleScraper()
{
    FreeResources();

}