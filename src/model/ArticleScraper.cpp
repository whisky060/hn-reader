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
    if (doc == nullptr)
    {
        FreeResources();
        throw HTMLParsingException("couldn't allocate enough space during parsing creation");
    }

    auto code = lxb_html_document_parse(doc, (const lxb_char_t *)html_page.data(), html_page.size());
    if (code != LXB_STATUS_OK)
    {
        FreeResources();
        throw HTMLParsingException("couldn't parse the html page");
    }

    dom = lxb_dom_interface_node(lxb_html_document_body_element(doc));
}

void ArticleScraper::FreeResources()
{
    if (doc != nullptr)
        lxb_html_document_destroy(doc);
    
    doc = nullptr;

    if (dom != nullptr)
        lxb_dom_node_destroy(dom);
    
    dom = nullptr;
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

lxb_dom_node_t *ArticleScraper::GetParent(lxb_dom_node_t *node)
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
    lxb_char_t *text = lxb_dom_node_text_content(node, &len);
    std::string result(reinterpret_cast<const char *>(text), len);

    lxb_dom_document_destroy_text(node->owner_document, text);

    return result;
}

bool ArticleScraper::IsElementNode(lxb_dom_node_t *node)
{
    if (node == nullptr)
        return false;

    return node->type == LXB_DOM_NODE_TYPE_ELEMENT;
}

bool ArticleScraper::IsTextNode(lxb_dom_node_t *node)
{
    if (node == nullptr)
        return false;

    return node->type == LXB_DOM_NODE_TYPE_TEXT;
}

std::string ArticleScraper::GetTagName(lxb_dom_node_t *node)
{
    if (node == nullptr || !IsElementNode(node))
    {
        return "";
    }

    size_t len = 0;
    const lxb_char_t *name = lxb_dom_element_qualified_name_upper(lxb_dom_interface_element(node), &len);
    std::string result(reinterpret_cast<const char *>(name), len);

    return result;
}

std::string ArticleScraper::GetNodeId(lxb_dom_node_t *node)
{
    if (node == nullptr || !IsElementNode(node))
    {
        return "";
    }

    size_t len = 0;
    const lxb_char_t *id = lxb_dom_element_id(lxb_dom_interface_element(node), &len);
    std::string result(reinterpret_cast<const char *>(id), len);

    return result;
}

std::string ArticleScraper::GetNodeClass(lxb_dom_node_t *node)
{
    if (node == nullptr || !IsElementNode(node))
    {
        return "";
    }

    size_t len = 0;
    const lxb_char_t *cls = lxb_dom_element_class(lxb_dom_interface_element(node), &len);
    std::string result(reinterpret_cast<const char *>(cls), len);

    return result;
}

std::string ArticleScraper::GetAttribute(lxb_dom_node_t *node, const std::string &attr_name)
{
    if (node == nullptr || !IsElementNode(node))
    {
        return "";
    }

    size_t len = 0;
    const lxb_char_t *attr = lxb_dom_element_get_attribute(lxb_dom_interface_element(node),
                                                           reinterpret_cast<const lxb_char_t *>(const_cast<char *>(attr_name.c_str())),
                                                           len, nullptr);
    std::string result(reinterpret_cast<const char *>(attr), len);

    return result;
}

bool ArticleScraper::HasAttribute(lxb_dom_node_t *node, const std::string &attr_name)
{
    if (node == nullptr || !IsElementNode(node))
    {
        return false;
    }

    const lxb_char_t *attr = lxb_dom_element_get_attribute(lxb_dom_interface_element(node),
                                                           reinterpret_cast<const lxb_char_t *>(const_cast<char *>(attr_name.c_str())),
                                                           0, nullptr);
    return attr != nullptr;
}

void ArticleScraper::SetAttribute(lxb_dom_node_t *node, const std::string &attr_name, const std::string &value)
{
    if (node == nullptr || !IsElementNode(node))
    {
        return;
    }

    lxb_dom_element_set_attribute(lxb_dom_interface_element(node),
                                  reinterpret_cast<const lxb_char_t *>(const_cast<char *>(attr_name.c_str())),
                                  attr_name.size(),
                                  reinterpret_cast<const lxb_char_t *>(const_cast<char *>(value.c_str())),
                                  value.size());
}

void ArticleScraper::RemoveAttribute(lxb_dom_node_t *node, const std::string &attr_name)
{
    if (node == nullptr || !IsElementNode(node))
    {
        return;
    }

    lxb_dom_element_remove_attribute(lxb_dom_interface_element(node),
                                     reinterpret_cast<const lxb_char_t *>(const_cast<char *>(attr_name.c_str())),
                                     attr_name.size());
}

lxb_dom_node_t *ArticleScraper::GetFirstChild(lxb_dom_node_t *node)
{
    return node->first_child;
}

lxb_dom_node_t *ArticleScraper::GetNextSibling(lxb_dom_node_t *node)
{
    if (node == nullptr)
        return nullptr;

    return node->next;
}

lxb_dom_node_t *ArticleScraper::GetPrevSibling(lxb_dom_node_t *node)
{
    if (node == nullptr)
        return nullptr;

    return node->prev;
}

lxb_dom_node_t *ArticleScraper::GetFirstChildElement(lxb_dom_node_t *node)
{
    if (node == nullptr)
    {
        return nullptr;
    }

    lxb_dom_node_t *child = node->first_child;
    while (child != nullptr)
    {
        if (IsElementNode(child))
        {
            return child;
        }
        child = child->next;
    }

    return nullptr;
}

// Get next ELEMENT sibling (skips text/comment nodes)
lxb_dom_node_t *ArticleScraper::GetNextElementSibling(lxb_dom_node_t *node)
{
    if (node == nullptr)
        return nullptr;

    lxb_dom_node_t *sibling = node->next;
    while (sibling != nullptr)
    {
        if (IsElementNode(sibling))
        {
            return sibling;
        }
        sibling = sibling->next;
    }

    return nullptr;
}

// Get previous ELEMENT sibling
lxb_dom_node_t *ArticleScraper::GetPrevElementSibling(lxb_dom_node_t *node)
{
    if (node == nullptr)
        return nullptr;

    lxb_dom_node_t *sibling = node->prev;
    while (sibling != nullptr)
    {
        if (IsElementNode(sibling))
        {
            return sibling;
        }
        sibling = sibling->prev;
    }

    return nullptr;
}

// Count element children
size_t ArticleScraper::GetChildElementCount(lxb_dom_node_t *node)
{
    if (node == nullptr)
    {
        return 0;
    }

    size_t count = 0;
    lxb_dom_node_t *child = node->first_child;
    while (child != nullptr)
    {
        if (IsElementNode(child))
        {
            count++;
        }
        child = child->next;
    }

    return count;
}

// Depth-first traversal (skip self if ignoreSelfAndKids=true)
lxb_dom_node_t *ArticleScraper::GetNextNode(lxb_dom_node_t *node, bool ignoreSelfAndKids = false)
{
    if (node == nullptr)
        return nullptr;

    if (!ignoreSelfAndKids && GetFirstChildElement(node))
        return GetFirstChildElement(node);

    if (GetNextSibling(node))
        return GetNextSibling(node);

    // move up the parent chain *and* find a sibling
    // (because this is depth-first traversal, we will have already
    // seen the parent nodes themselves).
    do
    {
        node = node->parent;
    } while (node && !GetNextElementSibling(node));

    return node ? GetNextElementSibling(node) : nullptr;
}

// Get ancestor chain (up to maxDepth, 0 = unlimited)
std::vector<lxb_dom_node_t *> ArticleScraper::GetAncestors(lxb_dom_node_t *node, size_t maxDepth = 0)
{
    std::vector<lxb_dom_node_t *> ancestors;
    size_t depth = 0;

    while (GetParent(node))
    {
        ancestors.push_back(GetParent(node));
        if (++depth == maxDepth)
            break;
        node = GetParent(node);
    }
    return ancestors;
}

// Remove node from tree
void ArticleScraper::RemoveNode(lxb_dom_node_t *node)
{
    if (node == nullptr)
        return;

    lxb_dom_node_remove(node);
}

// Remove and return the next node (for safe iteration during removal)
lxb_dom_node_t *ArticleScraper::RemoveAndGetNext(lxb_dom_node_t *node)
{
    if (node == nullptr)
        return nullptr;
    auto next_node = GetNextNode(node);
    RemoveNode(node);
    return next_node;
}

// Replace child: parent replaces oldChild with newChild
void ArticleScraper::ReplaceChild(lxb_dom_node_t *parent, lxb_dom_node_t *newChild, lxb_dom_node_t *oldChild)
{
    if (parent == nullptr || newChild == nullptr || oldChild == nullptr)
        return;

    lxb_dom_node_insert_before(oldChild, newChild);
    lxb_dom_node_remove(oldChild);
    lxb_dom_node_destroy(oldChild); // actually frees it
}

// Append child to parent
void ArticleScraper::AppendChild(lxb_dom_node_t *parent, lxb_dom_node_t *child)
{
    if (parent == nullptr || child == nullptr)
        return;

    lxb_dom_node_append_child(parent, child);
}

// Insert newChild before refChild under parent
void ArticleScraper::InsertBefore(lxb_dom_node_t *parent, lxb_dom_node_t *newChild, lxb_dom_node_t *refChild)
{
    if (parent == nullptr || newChild == nullptr || refChild == nullptr)
        return;

    lxb_dom_node_insert_before_spec(parent, newChild, refChild);
}

// Create element with given tag name
lxb_dom_node_t *ArticleScraper::CreateElement(const std::string &tagName)
{

    auto element = lxb_dom_document_create_element(&doc->dom_document, reinterpret_cast<const lxb_char_t *>(tagName.c_str()), tagName.size());

    if (element == nullptr)
        return nullptr;

    return lxb_dom_interface_node(element);
}

// Create text node
lxb_dom_node_t *ArticleScraper::CreateTextNode(const std::string &text)
{
    auto text_node = lxb_dom_document_create_text_node(&doc->dom_document, reinterpret_cast<const lxb_char_t *>(text.c_str()), text.size());
    if (text_node == nullptr)
        return nullptr;
    return lxb_dom_interface_node(text_node);
}

// Change a node's tag (create new element, move children + attrs, replace in tree)
lxb_dom_node_t *ArticleScraper::SetNodeTag(lxb_dom_node_t *node, const std::string &newTag)
{
    auto replacement = CreateElement(newTag);

    if (replacement == nullptr)
        return node;

    while (GetFirstChild(node))
    {
        auto child = GetFirstChild(node);
        AppendChild(replacement, child);
    }

    auto parent = GetParent(node);
    ReplaceChild(parent, replacement, node);

    /*
        TODO 
         if (node.readability) {
                replacement.readability = node.readability;
        }

    */
    if(IsElementNode(node))
    {
        auto old_element = lxb_dom_interface_element(node);
        auto new_element = lxb_dom_interface_element(replacement); 

        auto attr = lxb_dom_element_first_attribute(old_element);
        while (attr != nullptr)
        {   
            size_t name_len = 0;
            size_t value_len = 0;
            const auto name = lxb_dom_attr_qualified_name(attr, &name_len);
            const auto value = lxb_dom_attr_value(attr, &value_len);

            if(name != nullptr)
            {   
                std::string name_str(reinterpret_cast<const char *>(name), name_len);
                std::string value_str(reinterpret_cast<const char *>(value), value_len);
                SetAttribute(replacement, name_str, value_str);
            }
            attr = lxb_dom_element_next_attribute(attr);
        }
        
    }

    return replacement;

}



// Get inner text of a node (all text descendants, whitespace normalized optionally)
std::string ArticleScraper::GetInnerText(lxb_dom_node_t *node, bool normalize = true)
{
    if(node == nullptr)
        throw std::invalid_argument("Node is null");

    if(!IsElementNode(node))
        throw std::invalid_argument("Node is not an element node");
    
    auto textContent = GetNodeTextContent(node);
    Trim(textContent);        

    if(normalize)
    {
        auto pattern = REGEXPS.at(RegexPattern::NORMALIZE);
        textContent = std::regex_replace(textContent, pattern, " ");
    }


    return textContent;
}


void ArticleScraper::Trim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}


// Get link density: ratio of text inside <a> tags vs total text
float ArticleScraper::GetLinkDensity(lxb_dom_node_t *node)
{
    float textLength = GetInnerText(node).size();
    if( textLength == 0 )
        return  0;
    float linkLength = 0;

    auto elements = GetElementsByTagName(node, "a");
    for(auto linknode : elements)
    {
        auto href = GetAttribute(linknode, "href");
        float coefficient = (href != "") && std::regex_match(href, REGEXPS.at(RegexPattern::HASH_URL)) ? 0.3f : 1.0f;
        linkLength += GetInnerText(linknode).size() * coefficient;
    }

    return linkLength / textLength;
}

// Get class weight: +/-25 per positive/negative regex match on class+id
int ArticleScraper::GetClassWeight(lxb_dom_node_t *node)
{
    int weight = 0;
    std::string className = GetNodeClass(node);
    std::string idName = GetNodeId(node);

    if(!className.empty())
    {
        if(std::regex_search(className, REGEXPS.at(RegexPattern::POSITIVE)))
            weight += 25;
        if(std::regex_search(className, REGEXPS.at(RegexPattern::NEGATIVE)))
            weight -= 25;
    }

    if(!idName.empty())
    {
        if(std::regex_search(idName, REGEXPS.at(RegexPattern::POSITIVE)))
            weight += 25;
        if(std::regex_search(idName, REGEXPS.at(RegexPattern::NEGATIVE)))
            weight -= 25;
    }

    return weight;
}


// Count occurrences of a character in node's text
int ArticleScraper::GetCharCount(lxb_dom_node_t *node, char delimiter = ',')
{
    return std::count(GetInnerText(node).begin(), GetInnerText(node).end(), delimiter);
}


// Get text density: ratio of text in specified child tags vs total
float ArticleScraper::GetTextDensity(lxb_dom_node_t *node, const std::vector<std::string> &tags)
{
    float totalTextLength = GetInnerText(node).size();
    if(totalTextLength == 0)
        return 0;

    float childTextLength = 0;
    for(const auto& tag : tags)
    {
        auto elements = GetElementsByTagName(node, tag);
        for(auto childNode : elements)
        {
            childTextLength += GetInnerText(childNode).size();
        }
    }

    return childTextLength / totalTextLength;
}


bool ArticleScraper::EveryNode(std::vector<lxb_dom_node_t *> nodes, std::function<bool(lxb_dom_node_t *)> predicate)
{
    for(auto node : nodes)
    {
        if(!predicate(node))
            return false;
    }
    return true;
}

// Is this node phrasing content? (inline elements, text nodes, etc.)
bool ArticleScraper::IsPhrasingContent(lxb_dom_node_t *node)
{
    if(node == nullptr || !IsElementNode(node))
        return false;    


    return IsTextNode(node) || 
        PHRASING_ELEMS.find(GetTagName(node)) != PHRASING_ELEMS.end() ||
        ((GetTagName(node) == "A" || GetTagName(node) == "DEL" || GetTagName(node) == "INS") && 
            EveryNode(GetChildElements(node), [this](lxb_dom_node_t *child) { return IsPhrasingContent(child); }));
}



// Is this node whitespace? (empty text node or <br>)
bool ArticleScraper::IsWhitespace(lxb_dom_node_t *node)
{
    if(node == nullptr)
        return false;

    if(IsTextNode(node))
    {
        std::string text = GetNodeTextContent(node);
        Trim(text);
        return text.empty();
    }

    if(IsElementNode(node) && GetTagName(node) == "BR")
        return true;
}


// Is this element empty? (no text, no meaningful children)
bool ArticleScraper::IsElementWithoutContent(lxb_dom_node_t *node)
{
    if(IsElementNode(node))
    {
        std::string text = GetInnerText(node);
        Trim(text);

        if(!text.empty())
            return false;

        auto children = GetChildElements(node);
        return !children.empty() || children.size() == GetElementsByTagName(node, "br").size() + GetElementsByTagName(node, "hr").size();
    }

    return false;
}

bool ArticleScraper::SomeNode(std::vector<lxb_dom_node_t *> nodes, std::function<bool(lxb_dom_node_t *)> predicate)
{
    for(auto node : nodes)
    {
        if(predicate(node))
            return true;
    }
    return false;
}


// Does this element have any block-level child elements?
bool ArticleScraper::HasChildBlockElement(lxb_dom_node_t *element)
{

    if(!IsElementNode(element))
        return false;

    return SomeNode(GetChildElements(element), [this](lxb_dom_node_t *child) { 
        return DIV_TO_P_ELEMS.find(GetTagName(child)) != DIV_TO_P_ELEMS.end() 
            || HasChildBlockElement(child);
    });
}


