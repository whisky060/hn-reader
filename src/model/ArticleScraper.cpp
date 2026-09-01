#include "model/ArticleScraper.hpp"


std::string ArticleScraper::operator()(std::string html_page)
{   
    ParseHTML(html_page);
    ExtractContent();
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
    UnwrapNoScriptImages();
}


/**
* Find all <noscript> that are located after <img> nodes, and which contain only one
* <img> element. Replace the first image with the image from inside the <noscript> tag,
* and remove the <noscript> tag. This improves the quality of the images we use on
* some sites (e.g. Medium).
*
* 
**/
void ArticleScraper::UnwrapNoScriptImages()
{
    
}

ArticleScraper::~ArticleScraper()
{
    FreeResources();
}

std::vector<lxb_dom_node_t *> ArticleScraper::GetElementsByTagName(lxb_dom_node_t *root, const std::string &tag_name)
{
    std::vector<lxb_dom_node_t *> nodes;
    lxb_dom_node_t *current = GetFirstChild(root);
    while (current != nullptr)
    {
        if (GetTagName(current) == tag_name)
        {
            nodes.push_back(current);
        }
        current = GetNextSibling(current);
    }
    return nodes;
}

 std::vector<lxb_dom_node_t *> ArticleScraper::GetAllNodesWithTag(lxb_dom_node_t *root, std::unordered_set<std::string> tag_names)
 {
    std::vector<lxb_dom_node_t *> nodes;
    lxb_dom_node_t *current = GetFirstChild(root);
    while (current != nullptr)
    {
        if (tag_names.find(GetTagName(current)) != tag_names.end())
        {
            nodes.push_back(current);
        }
        current = GetNextSibling(current);
    }
    return nodes;
 }

 lxb_dom_node_t * ArticleScraper::GetParent(lxb_dom_node_t *node)
 {
    return node->parent;
 }

 std::vector<lxb_dom_node_t *> ArticleScraper::GetChildElements(lxb_dom_node_t *node)
 {
    std::vector<lxb_dom_node_t *> elements;
    lxb_dom_node_t *current = GetFirstChild(node);
    while (current != nullptr)
    {
        if (IsElementNode(current))
        {
            elements.push_back(current);
        }
        current = GetNextSibling(current);
    }
    return elements;
 }

  std::string ArticleScraper::GetNodeTextContent(lxb_dom_node_t *node)
  {
    size_t len = 0;
    lxb_char_t* text = lxb_dom_node_text_content(node, &len);
    std::string result(reinterpret_cast<const char*>(text), len);

    lxb_dom_document_destroy_text(node->owner_document, text);

    return result;
  }

  bool ArticleScraper::IsElementNode(lxb_dom_node_t *node)
  {
    return node->type == LXB_DOM_NODE_TYPE_ELEMENT;
  }

  bool ArticleScraper::IsTextNode(lxb_dom_node_t *node)
  {
    return node->type == LXB_DOM_NODE_TYPE_TEXT;
  }

  std::string ArticleScraper::GetTagName(lxb_dom_node_t *node)
  {
    size_t len = 0;
    lxb_char_t* tag_name = lxb_dom_node_tag_name(node, &len);
    std::string result(reinterpret_cast<const char*>(tag_name), len);

    lxb_dom_document_destroy_text(node->owner_document, tag_name);

    return result;
  }