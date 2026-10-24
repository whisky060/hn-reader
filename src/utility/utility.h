#ifndef UTILITY_H
#define UTILITY_H

#include <algorithm>
#include <filesystem>
#include <string>
#include <cstdlib>
#include "model/ContentBlock/ContentBlock.hpp"
#include "lexbor/dom/interface.h"
#include <lexbor/html/html.h>
#include "exeptions/html_parsing_error.hpp"



#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#endif

const std::string appName = "hn-reader";
const std::string configName = "configuration.json";
const std::string themeDirName = "themes";

namespace utils {

    using date = char*;
    date GetCurrentDate();
    date GetDateFromUnix(long long timestamp);
    void OpenUrl(const std::string& url);
    void Trim(std::string &s);
    std::filesystem::path GetThemeDir();
    std::filesystem::path GetConfigFile();

    class PlainHTMLPArsing
    {
      public:
      PlainHTMLPArsing() = default;
      ~PlainHTMLPArsing(){FreeResources();};
      ContentBlock operator()(std::string page)
      {

        doc = lxb_html_document_create();
        if (doc == nullptr) {
            FreeResources();
            throw HTMLParsingException(
                "couldn't allocate enough space during parsing creation");
        }

        auto code = lxb_html_document_parse(doc, (const lxb_char_t *)page.data(),
                                            page.size());
        if (code != LXB_STATUS_OK) {
          FreeResources();
          throw HTMLParsingException("couldn't parse the html page");
        }

        dom = lxb_dom_interface_node(lxb_html_document_body_element(doc));
        root = lxb_dom_interface_node(doc->dom_document.element);
        std::vector<ContentBlock> blocks;
        CollectBlocks(root, blocks);
        FreeResources();
        ContentBlock result(ContentBlockType::DIV, "", "", blocks);
        return result;
      }
      private:
        void CollectBlocks(lxb_dom_node_t *node, std::vector<ContentBlock>& blocks)
        {
            if (node == nullptr)
                return;
            auto tag = GetTagName(node);
            auto child = GetFirstChild(node);
            auto type = GetContentBlockType(tag);
            std::vector<ContentBlock> children;
            if(IsContainerNode(type))
            {
                for(; child != nullptr; child = GetNextSibling(child))
                {
                    CollectBlocks(child, children);
                }
                switch (type) {
                	case ContentBlockType::A:
                 		blocks.push_back({type, "", GetAttribute(node, "href"), children});
                 		break;
                	default:
                 		blocks.push_back({type, "", "", children});
                 		break;
                }
            }
            else
            {
                switch (type) {
                	case ContentBlockType::IMG:
                 		blocks.push_back({type, "", GetAttribute(node, "src"), children});
                 		break;
                	case ContentBlockType::TEXT:
                 		blocks.push_back({type, GetNodeTextContent(node, false), "", children});
                   		break;
                 	case ContentBlockType::BR:
                 	case ContentBlockType::HR:
                 		blocks.push_back({type, "", "", children});
                 		break;
                 	default:
                  		break;
                }
            }

            return;
        }
        std::string GetNodeTextContent(lxb_dom_node_t *node, bool trim) {
          size_t len = 0;
          lxb_char_t *text = lxb_dom_node_text_content(node, &len);
          if(text == nullptr)
              return "";
          std::string result(reinterpret_cast<const char *>(text), len);

          lxb_dom_document_destroy_text(node->owner_document, text);

          if (trim) {
            Trim(result);
          }

          return result;
        }
        bool IsContainerNode(ContentBlockType type)
        {
          return (
            type == ContentBlockType::DIV ||
            type == ContentBlockType::PRE ||
            type == ContentBlockType::CODE ||
            type == ContentBlockType::TABLE ||
            type == ContentBlockType::UL ||
            type == ContentBlockType::I ||
            type == ContentBlockType::LI ||
            type == ContentBlockType::U ||
            type == ContentBlockType::H1 ||
            type == ContentBlockType::H2 ||
            type == ContentBlockType::H3 ||
            type == ContentBlockType::B ||
            type == ContentBlockType::TR ||
            type == ContentBlockType::TD ||
            type == ContentBlockType::CAPTION ||
            type == ContentBlockType::A ||
            type == ContentBlockType::P ||
            type == ContentBlockType::BLOCKQUOTE ||
            type == ContentBlockType::SPAN
          );
        }
        lxb_dom_node_t* GetFirstChild(lxb_dom_node_t *node)
        {
          if (node == nullptr)
            return nullptr;
          return node->first_child;
        }

        std::string GetAttribute(lxb_dom_node_t *node, const std::string &attr_name) {
          if (node == nullptr || !IsElementNode(node)) {
            return "";
          }

          size_t len = 0;
          const lxb_char_t *attr =
              lxb_dom_element_get_attribute(lxb_dom_interface_element(node),
                                            reinterpret_cast<const lxb_char_t *>(
                                                const_cast<char *>(attr_name.c_str())),
                                            attr_name.size(), &len);
          if (attr == nullptr) {
            return "";
          }
          std::string result(reinterpret_cast<const char *>(attr), len);

          return result;
        }
        ContentBlockType GetContentBlockType(std::string& tag)
        {
            if(tag == "TABLE")
                return ContentBlockType::TABLE;
            if(tag == "IMG")
                return ContentBlockType::IMG;
            if(tag == "CODE")
                return ContentBlockType::CODE;
            if(tag == "PRE")
                return ContentBlockType::PRE;
            if(tag == "BLOCKQUOTE")
                return ContentBlockType::BLOCKQUOTE;
            if(tag == "H1" || tag == "TITLE")
                return ContentBlockType::H1;
            if(tag == "H2")
                return ContentBlockType::H2;
            if(tag == "H3" || tag == "H4" || tag == "H5" || tag == "H6")
                return ContentBlockType::H3;
            if(tag == "P")
                return ContentBlockType::P;
            if(tag == "LI")
                return ContentBlockType::LI;
            if(tag == "UL" || tag == "OL" || tag == "DL")
                return ContentBlockType::UL;
            if(tag == "A")
                return ContentBlockType::A;
            if(tag == "B" || tag == "STRONG" || tag == "BOLD")
                return ContentBlockType::B;
            if(tag == "I" || tag == "EM")
                return ContentBlockType::I;
            if(tag == "U" || tag == "UNDERLINE")
                return ContentBlockType::U;
            if(tag == "DIV" || tag == "ARTICLE" ||
               tag == "SECTION" || tag == "ASIDE" ||
               tag == "MAIN" || tag == "HEADER" ||
               tag == "FOOTER" || tag == "FIGURE" ||
               tag == "PICTURE" || tag == "SOURCE" || tag == "HTML" || tag == "BODY")
                return ContentBlockType::DIV;
            if(tag == "TR")
                return ContentBlockType::TR;
            if(tag == "TD" || tag == "TH")
                return ContentBlockType::TD;
            if(tag == "CAPTION" || tag == "FIGCAPTION")
                return ContentBlockType::CAPTION;
            if (tag == "BR")
                return ContentBlockType::BR;
            if (tag == "HR")
                return ContentBlockType::HR;
            if(tag == "SPAN")
                return ContentBlockType::SPAN;

            return ContentBlockType::TEXT;

        }
        lxb_dom_node_t* GetNextSibling(lxb_dom_node_t *node)
        {
          if (node == nullptr)
            return nullptr;

          return node->next;
        }
        std::string GetTagName(lxb_dom_node_t *node) {
          if (node == nullptr || !IsElementNode(node)) {
            return "";
          }

          size_t len = 0;
          const lxb_char_t *name = lxb_dom_element_qualified_name_upper(
              lxb_dom_interface_element(node), &len);
          std::string result(reinterpret_cast<const char *>(name), len);
          std::transform(result.begin(),result.end(),result.begin(),[](char ch){return toupper(ch);});
          return result;
        }
        bool IsElementNode(lxb_dom_node_t *node) {
          if (node == nullptr)
            return false;

          return node->type == LXB_DOM_NODE_TYPE_ELEMENT;
        }

        void FreeResources()
        {

            if (doc != nullptr)
            {
                lxb_html_document_destroy(doc);
                doc = nullptr;
                dom = nullptr;
                root = nullptr;
            }
        }

        lxb_html_document_t *doc;
        lxb_dom_node_t *dom;
        lxb_dom_node_t* root;
    };
}



#endif
