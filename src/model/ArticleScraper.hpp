#ifndef ARTICLESCRAPER_H
#define ARTICLESCRAPER_H

#include <lexbor/html/html.h>
#include <string>
#include <thread>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <vector>
#include <functional>
#include <algorithm>
#include "exeptions/html_parsing_error.hpp"
#include "lexbor/dom/interface.h"
#include "model/Logger.hpp"

class ArticleScraper
{
private:
    void ParseHTML(std::string html_page);
    void RemoveScripts();
    void PrepDocument();

    lxb_dom_element_t *GrabArticle(lxb_dom_node_t *page);
    void UnwrapNoScriptImages();
    void FreeResources();
    std::string GetArticleTitle();

    void PrepArticle(lxb_dom_node_t *articleContent);
    void CleanStyles(lxb_dom_node_t *element);
    void MarkDataTables(lxb_dom_node_t *root);
    void CleanConditionally(lxb_dom_node_t* node, std::string tag);
    bool HasVideoAttribute(lxb_dom_node_t* node);
    void Clean(lxb_dom_node_t* node, std::string tag);
    void CleanMatchedNode(lxb_dom_node_t* node, std::function<bool(lxb_dom_node_t*, std::string)> filter);
    void CleanHeaders(lxb_dom_node_t* element);
    void RemoveNodes(std::vector<lxb_dom_node_t*> nodesToRemove, std::function<bool(lxb_dom_node_t*)> filter);
    void FixRelativeUris(lxb_dom_node_t* articleContent);
    void SimplifyNestedElements(lxb_dom_node_t* articleContent);
    void PostProcessContent(lxb_dom_node_t* articleContent);
    void CleanClasses(lxb_dom_node_t* node);

    // helpers
    std::vector<lxb_dom_node_t *> GetElementsByTagName(lxb_dom_node_t *root, const std::string &tag_name);
    std::vector<lxb_dom_node_t *> GetAllNodesWithTag(lxb_dom_node_t *root, std::unordered_set<std::string> tag_names);
    lxb_dom_node_t *GetParent(lxb_dom_node_t *node);
    std::vector<lxb_dom_node_t *> GetChildElements(lxb_dom_node_t *node);
    std::vector<lxb_dom_node_t *> GetNodes(lxb_dom_node_t *root);
    std::string GetNodeTextContent(lxb_dom_node_t *node);
    bool IsElementNode(lxb_dom_node_t *node);
    bool IsTextNode(lxb_dom_node_t *node);
    bool IsValidByLine(lxb_dom_node_t *node, std::string &matchString);



    // Get tag name (uppercase, e.g. "DIV")
    std::string GetTagName(lxb_dom_node_t *node);

    std::string GetNodeId(lxb_dom_node_t *node);
    std::string GetNodeClass(lxb_dom_node_t *node);
    std::string GetAttribute(lxb_dom_node_t *node, const std::string &attr_name);
    bool HasAttribute(lxb_dom_node_t *node, const std::string &attr_name);
    void SetAttribute(lxb_dom_node_t *node, const std::string &attr_name, const std::string &value);
    void RemoveAttribute(lxb_dom_node_t *node, const std::string &attr_name);
    lxb_dom_node_t *GetFirstChild(lxb_dom_node_t *node);
    lxb_dom_node_t *GetLastChild(lxb_dom_node_t *node);
    lxb_dom_node_t *GetNextSibling(lxb_dom_node_t *node);
    lxb_dom_node_t *GetPrevSibling(lxb_dom_node_t *node);

    void ReplaceNodeTags(std::vector<lxb_dom_node_t *> nodes, const std::string &newTag);

    bool HeaderDuplicateTitle(lxb_dom_node_t *element);
    double TextSimilarity(std::string textA, std::string textB);

    // Get first ELEMENT child (skips text/comment nodes)
    lxb_dom_node_t *GetFirstChildElement(lxb_dom_node_t *node);

    // Get next ELEMENT sibling (skips text/comment nodes)
    lxb_dom_node_t *GetNextElementSibling(lxb_dom_node_t *node);

    // Get previous ELEMENT sibling
    lxb_dom_node_t *GetPrevElementSibling(lxb_dom_node_t *node);

    // Count element children
    size_t GetChildElementCount(lxb_dom_node_t *node);

    // Depth-first traversal (skip self if ignoreSelfAndKids=true)
    lxb_dom_node_t *GetNextNode(lxb_dom_node_t *node, bool ignoreSelfAndKids = false);

    // Get ancestor chain (up to maxDepth, 0 = unlimited)
    std::vector<lxb_dom_node_t *> GetAncestors(lxb_dom_node_t *node, size_t maxDepth = 0);

    // Remove node from tree
    void RemoveNode(lxb_dom_node_t *node);
    void RemoveChilde(lxb_dom_node_t* parent, lxb_dom_node_t* child);

    // Remove and return the next node (for safe iteration during removal)
    lxb_dom_node_t *RemoveAndGetNext(lxb_dom_node_t *node);

    // Replace child: parent replaces oldChild with newChild
    void ReplaceChild(lxb_dom_node_t *parent, lxb_dom_node_t *newChild, lxb_dom_node_t *oldChild);

    // Append child to parent
    void AppendChild(lxb_dom_node_t *parent, lxb_dom_node_t *child);

    // Insert newChild before refChild under parent
    void InsertBefore(lxb_dom_node_t *parent, lxb_dom_node_t *newChild, lxb_dom_node_t *refChild);

    // Create element with given tag name
    lxb_dom_node_t *CreateElement(const std::string &tagName);

    // Create text node
    lxb_dom_node_t *CreateTextNode(const std::string &text);

    // Change a node's tag (create new element, move children + attrs, replace in tree)
    lxb_dom_node_t *SetNodeTag(lxb_dom_node_t *node, const std::string &newTag);

    // Get inner text of a node (all text descendants, whitespace normalized optionally)
    std::string GetInnerText(lxb_dom_node_t *node, bool normalize = true);

    // Get link density: ratio of text inside <a> tags vs total text
    float GetLinkDensity(lxb_dom_node_t *node);

    // Get class weight: +/-25 per positive/negative regex match on class+id
    int GetClassWeight(lxb_dom_node_t *node);

    // Count occurrences of a character in node's text
    int GetCharCount(lxb_dom_node_t *node, char delimiter = ',');

    // Get text density: ratio of text in specified child tags vs total
    float GetTextDensity(lxb_dom_node_t *node, const std::vector<std::string> &tags);

    bool EveryNode(std::vector<lxb_dom_node_t *> nodes, std::function<bool(lxb_dom_node_t *)> predicate);

    bool SomeNode(std::vector<lxb_dom_node_t *> nodes, std::function<bool(lxb_dom_node_t *)> predicate);

    void ReplaceBrs(lxb_dom_node_t *element);

    // Is this node phrasing content? (inline elements, text nodes, etc.)
    bool IsPhrasingContent(lxb_dom_node_t *node);

    // Is this node whitespace? (empty text node or <br>)
    bool IsWhitespace(lxb_dom_node_t *node);

    // Is this element empty? (no text, no meaningful children)
    bool IsElementWithoutContent(lxb_dom_node_t *node);

    // Does this element have any block-level child elements?
    bool HasChildBlockElement(lxb_dom_node_t *element);

    // Does this element have exactly one child of given tag, with no real text?
    bool HasSingleTagInsideElement(lxb_dom_node_t *element, const std::string &tag);

    // Does this node have an ancestor with the given tag name (up to maxDepth)?
    bool HasAncestorTag(lxb_dom_node_t *node, const std::string &tagName, size_t maxDepth = 3, std::function<bool(lxb_dom_node_t *)> filter = nullptr);

    // Is this node probably visible? (not display:none, not hidden, etc.)
    bool IsProbablyVisible(lxb_dom_node_t *node);

    std::string GetStyleAttribute(lxb_dom_node_t *node, const std::string &property);

    // Initialize a node with base content score from its tag type
    void InitializeNode(lxb_dom_node_t *node);

    // Get innerHTML of a node (serialize subtree to string)
    std::string GetInnerHtml(lxb_dom_node_t *node);
    void SetInnerHtml(lxb_dom_node_t *node, const std::string &html);


    void Trim(std::string &s);
    static std::string ToUpper(const std::string &s);

    enum class RegexPattern
    {
        UNLIKELY_CANDIDATES,
        OK_MAYBE_ITS_A_CANDIDATE,
        POSITIVE,
        NEGATIVE,
        EXTRANEOUS,
        BYLINE,
        REPLACE_FONTS,
        NORMALIZE,
        VIDEOS,
        SHARE_ELEMENTS,
        NEXT_LINK,
        PREV_LINK,
        TOKENIZE,
        WHITESPACE,
        HAS_CONTENT,
        HASH_URL,
        SRC_STE_URL,
        B64_DATA_URL,
        COMMAS,
        JSON_LD_ARTICLE_TYPES,
        AD_WORDS,
        LOADING_WORDS
    };

    lxb_html_document_t *doc;
    lxb_dom_node_t *dom;
    lxb_dom_node_t* root;
    bool isDebugMode = true;
    std::string articleLang;
    std::string articleByline;
    std::string articleTitle;
    std::string articleDir;
    int nbTopCandidates;

    // Readability sieve flags (mirrors Readability.js FLAG_* 0x1|0x2|0x4)
    static constexpr int FLAG_STRIP_UNLIKELYS = 0x1;
    static constexpr int FLAG_WEIGHT_CLASSES = 0x2;
    static constexpr int FLAG_CLEAN_CONDITIONALLY = 0x4;
    int flags_ = FLAG_STRIP_UNLIKELYS | FLAG_WEIGHT_CLASSES |
                 FLAG_CLEAN_CONDITIONALLY;

    struct Attempt {
      lxb_dom_node_t *articleContent = nullptr;
      size_t textLength = 0;
    };
    std::vector<Attempt> attempts_;

    bool IsFlagActive(int flag) const { return (flags_ & flag) > 0; }
    void RemoveFlag(int flag) { flags_ &= ~flag; }

    // Max number of nodes supported by this parser. Default: 0 (no limit)
    const int DEFAULT_MAX_ELEMS_TO_PARSE = 0;

    // The number of top candidates to consider when analysing how
    // tight the competition is among candidates.
    const int DEFAULT_N_TOP_CANDIDATES = 5;

    // Element tags to score by default (UPPER to match GetTagName()).
    const std::unordered_set<std::string> DEFAULT_TAGS_TO_SCORE = {
        "SECTION", "H2", "H3", "H4", "H5", "H6", "P", "TD", "PRE"};

    const std::unordered_set<std::string> UNLIKELY_ROLES = {
        "menu",
        "menubar",
        "complementary",
        "navigation",
        "alert",
        "alertdialog",
        "dialog"};

    const std::unordered_set<std::string> DIV_TO_P_ELEMS = {
        "BLOCKQUOTE",
        "DL",
        "DIV",
        "IMG",
        "OL",
        "P",
        "PRE",
        "TABLE",
        "UL"};

    const std::unordered_set<std::string> ALTER_TO_DIV_EXCEPTIONS = {"DIV", "ARTICLE", "SECTION", "P", "OL", "UL"};

    const std::vector<std::string> PRESENTATIONAL_ATTRIBUTES = {
        "align",
        "background",
        "bgcolor",
        "border",
        "cellpadding",
        "cellspacing",
        "frame",
        "hspace",
        "rules",
        "style",
        "valign",
        "vspace",
    };

    const std::unordered_set<std::string> DEPRECATED_SIZE_ATTRIBUTE_ELEMS = {"TABLE", "TH", "TD", "HR", "PRE"};

    // The commented out elements qualify as phrasing content but tend to be
    // removed by readability when put into paragraphs, so we ignore them here.
    const std::unordered_set<std::string> PHRASING_ELEMS = {
        // "CANVAS", "IFRAME", "SVG", "VIDEO",
        "ABBR",
        "AUDIO",
        "B",
        "BDO",
        "BR",
        "BUTTON",
        "CITE",
        "CODE",
        "DATA",
        "DATALIST",
        "DFN",
        "EM",
        "EMBED",
        "I",
        "IMG",
        "INPUT",
        "KBD",
        "LABEL",
        "MARK",
        "MATH",
        "METER",
        "NOSCRIPT",
        "OBJECT",
        "OUTPUT",
        "PROGRESS",
        "Q",
        "RUBY",
        "SAMP",
        "SCRIPT",
        "SELECT",
        "SMALL",
        "SPAN",
        "STRONG",
        "SUB",
        "SUP",
        "TEXTAREA",
        "TIME",
        "VAR",
        "WBR",
    };

    const std::unordered_set<std::string> CLASSES_TO_PRESERVE = {"page"};

    const std::unordered_map<std::string, char> HTML_ESCAPE_MAP = {
        {"lt", '<'},
        {"gt", '>'},
        {"amp", '&'},
        {"quot", '\"'},
        {"apos", '\''},
    };

    const unsigned int DEFAULT_CHAR_THRESHOLD = 500;
    const unsigned int DEFAULT_CHARDEFAULT_CHAR_THRESHOLD = 500;
    unsigned int charThreshold_ = 500;

    const std::unordered_map<RegexPattern, std::regex> REGEXPS = {
        {RegexPattern::UNLIKELY_CANDIDATES, std::regex("-ad-|ai2html|banner|breadcrumbs|combx|comment|community|cover-wrap|"
                                                       "disqus|extra|footer|gdpr|header|legends|menu|related|remark|replies|"
                                                       "rss|shoutbox|sidebar|skyscraper|social|sponsor|supplemental|ad-break|"
                                                       "agegate|pagination|pager|popup|yom-remote",
                                                       std::regex::icase)},
        {RegexPattern::OK_MAYBE_ITS_A_CANDIDATE, std::regex("and|article|body|column|content|main|mathjax|shadow", std::regex::icase)},
        {RegexPattern::POSITIVE, std::regex("article|body|content|entry|hentry|h-entry|main|page|pagination|post|text|blog|story", std::regex::icase)},
        {RegexPattern::NEGATIVE, std::regex("-ad-|hidden|^hid$| hid$| hid |^hid |banner|combx|comment|com-|contact|"
                                            "footer|gdpr|masthead|media|meta|outbrain|promo|related|scroll|share|"
                                            "shoutbox|sidebar|skyscraper|sponsor|shopping|tags|widget",
                                            std::regex::icase)},
        {RegexPattern::EXTRANEOUS, std::regex("print|archive|comment|discuss|e[\\-]?mail|share|reply|all|login|sign|single|utility", std::regex::icase)},
        {RegexPattern::BYLINE, std::regex("byline|author|dateline|writtenby|p-author", std::regex::icase)},
        {RegexPattern::REPLACE_FONTS, std::regex("<(/?)font[^>]*>", std::regex::icase)},
        {RegexPattern::NORMALIZE, std::regex("\\s{2,}", std::regex::icase)},
        {RegexPattern::VIDEOS, std::regex("//(www\\.)?((dailymotion|youtube|youtube-nocookie|player\\.vimeo|v\\.qq|"
                                          "bilibili|live.bilibili)\\.com|(archive|upload\\.wikimedia)\\.org|"
                                          "player\\.twitch\\.tv)",
                                          std::regex::icase)},
        {RegexPattern::SHARE_ELEMENTS, std::regex("(\\b|_)(share|sharedaddy)(\\b|_)", std::regex::icase)},
        {RegexPattern::NEXT_LINK, std::regex("(next|weiter|continue|>([^\\|]|$)|»([^\\|]|$))", std::regex::icase)},
        {RegexPattern::PREV_LINK, std::regex("(prev|earl|old|new|<|«)", std::regex::icase)},
        {RegexPattern::TOKENIZE, std::regex("\\W+", std::regex::icase)},
        {RegexPattern::WHITESPACE, std::regex("^\\s*$")},
        {RegexPattern::HAS_CONTENT, std::regex("^\\s*$")},
        {RegexPattern::HASH_URL, std::regex("^#.+")},
        {RegexPattern::SRC_STE_URL, std::regex("(\\S+)(\\s+[\\d.]+[xw])?(\\s*(?:,|$))", std::regex::icase)},
        {RegexPattern::B64_DATA_URL, std::regex("^data:\\s*([^\\s;,]+)\\s*;\\s*base64\\s*,", std::regex::icase)},
        {RegexPattern::COMMAS, std::regex(","             // U+002C  COMMA
                                          "|\xD8\x8C"     // U+060C  ARABIC COMMA
                                          "|\xEF\xB9\x90" // U+FE50  SMALL COMMA
                                          "|\xEF\xB8\x90" // U+FE10  PRESENTATION FORM FOR VERTICAL COMMA
                                          "|\xEF\xB8\x91" // U+FE11  PRESENTATION FORM FOR VERTICAL IDEOGRAPHIC COMMA
                                          "|\xE2\xB9\x81" // U+2E41  REVERSED COMMA
                                          "|\xE2\xB8\xB4" // U+2E34  RAISED COMMA
                                          "|\xE2\xB8\xB2" // U+2E32  TURNED COMMA
                                          "|\xEF\xBC\x8C")},
        {RegexPattern::JSON_LD_ARTICLE_TYPES, std::regex("^Article|AdvertiserContentArticle|NewsArticle|AnalysisNewsArticle|"
                                                         "AskPublicNewsArticle|BackgroundNewsArticle|OpinionNewsArticle|"
                                                         "ReportageNewsArticle|ReviewNewsArticle|Report|SatiricalArticle|"
                                                         "ScholarlyArticle|MedicalScholarlyArticle|SocialMediaPosting|BlogPosting|"
                                                         "LiveBlogPosting|DiscussionForumPosting|TechArticle|APIReference$")},
        {RegexPattern::AD_WORDS, std::regex("^(ad(vertising|vertisement)?|pub(licité)?|werb(ung)?|广告|Реклама|Anuncio)$", std::regex::icase)},
        {RegexPattern::LOADING_WORDS, std::regex("^((loading|正在加载|Загрузка|chargement|cargando)(…|\\.\\.\\.)?)$", std::regex::icase)}

    };

public:
    ArticleScraper() : doc(nullptr), dom(nullptr) {};
    ~ArticleScraper();
    std::string operator()(std::string html_page);
};

#endif
