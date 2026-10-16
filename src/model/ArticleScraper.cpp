#include "model/ArticleScraper.hpp"
#include "exeptions/scraping_exception.hpp"
#include "lexbor/dom/interface.h"
#include "model/ContentBlock.hpp"
#include "model/Logger.hpp"
#include <cstdio>
#include <cstdlib>
#include <format>
#include <math.h>
#include <sstream>
#include <string>
#include <unordered_map>

ContentBlock ArticleScraper::operator()(std::string html_page) {

    ParseHTML(html_page);

    articleByline = "";
    articleDir = "";
    articleTitle = GetArticleTitle();
    nbTopCandidates = DEFAULT_N_TOP_CANDIDATES;
    flags_ = FLAG_STRIP_UNLIKELYS | FLAG_WEIGHT_CLASSES | FLAG_CLEAN_CONDITIONALLY;
    charThreshold_ = DEFAULT_CHAR_THRESHOLD;
    attempts_.clear();

    RemoveScripts();
    PrepDocument();
    auto articleContent = GrabArticle(nullptr);
    if (articleContent == nullptr)
        throw ScrapingException("Failed to grab article");
    auto articleNode = lxb_dom_interface_node(articleContent);
    PostProcessContent(articleNode);


    std::vector<ContentBlock> blocks;
    CollectBlocks(articleNode, blocks);
    FreeResources();
    ContentBlock result(ContentBlockType::DIV, "", "", blocks);
    return result;
}


ContentBlockType ArticleScraper::GetContentBlockType(std::string& tag)
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
       tag == "PICTURE" || tag == "SOURCE")
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

bool ArticleScraper::IsLeafNode(ContentBlockType tag)
{
	return (
		tag == ContentBlockType::TEXT ||
		tag == ContentBlockType::IMG ||
		tag == ContentBlockType::BR ||
		tag == ContentBlockType::HR
	);
}

bool ArticleScraper::IsContainerNode(ContentBlockType type)
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

void ArticleScraper::CollectBlocks(lxb_dom_node_t *node, std::vector<ContentBlock>& blocks)
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

std::string ArticleScraper::GetNodeInfo(lxb_dom_node_t *node)
{
    auto tag = GetTagName(node);
    auto id = GetNodeId(node);
    auto classes = GetNodeClass(node);
    return std::format("Node id: {}, tag: {}, classes: {}", id, tag, classes);
}

void ArticleScraper::PostProcessContent(lxb_dom_node_t *articleContent) {


    if(isDebugMode)
        LOG_INFO("PostProcessContent on node: {}", GetNodeInfo(articleContent));

    FixRelativeUris(articleContent);
    SimplifyNestedElements(articleContent);
    CleanClasses(articleContent);

    if(isDebugMode)
        LOG_INFO("PostProcessContet finished");
}

void ArticleScraper::CleanClasses(lxb_dom_node_t *node) {
    if(isDebugMode)
        LOG_INFO("CleanClasses on node: {}", GetNodeInfo(node));

    auto classesToPreserve = CLASSES_TO_PRESERVE;
    auto className = GetNodeClass(node);
    std::stringstream ss(className);
    std::string token;
    std::vector<std::string> filtered;
    while (ss >> token) {
        if (classesToPreserve.find(token) != classesToPreserve.end()) {
            filtered.push_back(token);
        }
    }

    // than join back
    std::string filteredClassName = "";
    for (const auto &t : filtered) {
        filteredClassName += " ";
        filteredClassName += t;
    }

    if (!filteredClassName.empty()) {
        SetAttribute(node, "class", filteredClassName);
    } else {
        RemoveAttribute(node, "class");
    }
    for (auto node_ = GetFirstChildElement(node); node_;
         node_ = GetNextElementSibling(node_)) {
        CleanClasses(node_);
    }
}

void ArticleScraper::SimplifyNestedElements(lxb_dom_node_t *articleContent) {


    if(isDebugMode)
        LOG_INFO("SimplifyNestedElements on node: {}", GetNodeInfo(articleContent));

    auto node = articleContent;
    std::unordered_set<std::string> tagsToSimplify = {"DIV", "SECTION"};
    while (node) {
        if (GetParent(node) &&
            tagsToSimplify.find(GetTagName(node)) != tagsToSimplify.end() &&
            !(!GetNodeId(node).empty() && GetNodeId(node).starts_with("readability")))
        {
            if (IsElementWithoutContent(node))
            {
                node = RemoveAndGetNext(node);
                continue;
            }
            else if (HasSingleTagInsideElement(node, "DIV") ||
                    HasSingleTagInsideElement(node, "SECTION"))
            {
                auto child = GetChildElements(node)[0];
                auto *element = lxb_dom_interface_element(node);
                for (auto *a = lxb_dom_element_first_attribute(element); a;
                    a = lxb_dom_element_next_attribute(a))
                {
                    size_t nlen = 0, vlen = 0;
                    auto *n = lxb_dom_attr_qualified_name(a, &nlen);
                    auto *v = lxb_dom_attr_value(a, &vlen);
                    std::string name_str(reinterpret_cast<const char *>(n), nlen);
                    std::string value_str(reinterpret_cast<const char *>(v), vlen);
                    SetAttribute(child, name_str, value_str); // == child.setAttributeNode(clone)
                }
                auto parent = GetParent(node);
                ReplaceChild(parent, child, node);
                node = child;
                continue;
            }
        }
        node = GetNextNode(node);
    }
    if(isDebugMode)
        LOG_INFO("SimplifyNestedElemets finished on node : {}", GetNodeInfo(articleContent));
}

void ArticleScraper::FixRelativeUris(lxb_dom_node_t *articleContent) {

}

std::string ArticleScraper::GetArticleTitle() {
  std::string curTitle = "";
  std::string origTitle = "";
  size_t len;
  auto title = lxb_html_document_title(doc, &len);
  bool match;
  if (title) {
    auto titleStr = std::string(reinterpret_cast<const char *>(title), len);
    Trim(titleStr);
    curTitle = origTitle = titleStr;
  } else {
    auto titleNodes = GetElementsByTagName(root, "TITLE");
    if (!titleNodes.empty()) {
      curTitle = origTitle = GetInnerText(titleNodes[0]);
    }
  }

  bool titlehadHierarchicalSeparators = false;

  auto wordCount = [](std::string s) {
    std::stringstream ss(s);
    std::string w;
    size_t c = 0;
    while (ss >> w)
      c++;
    return c;
  };

  static const std::string titleSeparators = "\\|\\-\u2013\u2014\\\\/>\u00bb";
  std::regex sep_between_spaces("\\s[" + titleSeparators + "]\\s");

  if (std::regex_search(curTitle, sep_between_spaces)) {
    auto reg = std::regex("\\s[\\\\/>\u00bb]\\s");
    titlehadHierarchicalSeparators = std::regex_search(curTitle, reg);
    std::regex sep_between_spaces("\\s[" + titleSeparators + "]\\s");
    auto begin = std::sregex_iterator(curTitle.begin(), curTitle.end(),
                                      sep_between_spaces);
    auto end = std::sregex_iterator();
    std::sregex_iterator last_it;
    for (auto it = begin; it != end; ++it) {
      last_it = it;
    }
    curTitle = origTitle.substr(0, last_it->position());

    if (wordCount(curTitle) < 3) {
      std::regex leading_re("^[^" + titleSeparators + "]*[" + titleSeparators + "]");
      curTitle = std::regex_replace(origTitle, leading_re, "");
    }
  } else if (curTitle.find(": ") != std::string::npos) {
    // Check if we have an heading containing this exact string, so we
    // could assume it's the full title.
    auto headings = GetAllNodesWithTag(root, {"H1", "H2"});
    auto trimmedTitle = curTitle;
    Trim(trimmedTitle);
    match = SomeNode(headings, [this, trimmedTitle](lxb_dom_node_t *node) {
      auto text = GetNodeTextContent(node);
      Trim(text);
      return text == trimmedTitle;
    });

    if (!match) {
      curTitle = origTitle.substr(origTitle.find_last_of(":") + 1);
      if (wordCount(curTitle) < 3) {
        curTitle = origTitle.substr(origTitle.find(":") + 1);
      } else if (wordCount(origTitle.substr(0, origTitle.find(":"))) > 5) {
        curTitle = origTitle;
      }
    }
  } else if (curTitle.size() > 150 || curTitle.size() < 15) {
    auto hOnes = GetElementsByTagName(root, "H1");
    if (hOnes.size() == 1) {
      curTitle = GetInnerText(hOnes[0]);
    }
  }

  Trim(curTitle);
  curTitle =
      std::regex_replace(curTitle, REGEXPS.at(RegexPattern::NORMALIZE), " ");

  // If we now have 4 words or fewer as our title, and either no
  // 'hierarchical' separators (\, /, > or ») were found in the original
  // title or we decreased the number of words by more than 1 word, use
  // the original title.
  auto curTitleWordCount = wordCount(curTitle);

  if (curTitleWordCount <= 4 &&
      (!titlehadHierarchicalSeparators ||
       curTitleWordCount !=
           wordCount(std::regex_replace(origTitle, sep_between_spaces, "")) -
               1)) {
    curTitle = origTitle;
  }

  if(isDebugMode)
      LOG_INFO("Grabbed article title: {}", curTitle);

  return curTitle;
}

bool ArticleScraper::IsValidByLine(lxb_dom_node_t *node,
                                   std::string &matchString) {
  auto rel = GetAttribute(node, "rel");
  auto itemprop = GetAttribute(node, "itemprop");
  auto text = GetNodeTextContent(node);
  Trim(text);
  auto bylineLength = text.size();
  return (
        (rel == "author" ||
        itemprop.find("author") != std::string::npos ||
        std::regex_search(matchString, REGEXPS.at(RegexPattern::BYLINE))) &&
        bylineLength > 0 && bylineLength < 100);
}

lxb_dom_element_t *ArticleScraper::GrabArticle(lxb_dom_node_t *page) {

  auto isPaging = page != nullptr;

  page = page ? page : lxb_dom_interface_node(doc->body);

  if (page == nullptr) {
    return nullptr;
  }

  auto pageCacheHtml = GetInnerHtml(page);

  while (true) {
    auto stripUnlikelyCandidates = IsFlagActive(FLAG_STRIP_UNLIKELYS);
    // First, node prepping. Trash nodes that look cruddy (like ones with the
    // class name "comment", etc), and turn divs into P tags where they have
    // been used inappropriately (as in, where they contain no other block level
    // elements.)

    std::vector<lxb_dom_node_t *> elementsToScore;
    auto node = lxb_dom_interface_node(doc->dom_document.element);
    bool shouldRemoveTitleHeader = true;

    while (node) {
      if (GetTagName(node) == "HTML") {
        articleLang = GetAttribute(node, "lang");
      }

      auto matchString = GetNodeClass(node) + " " + GetNodeId(node);

      if (!IsProbablyVisible(node)) {
        node = RemoveAndGetNext(node);
        continue;
      }

      // User is not able to see elements applied with both "aria-modal = true"
      // and "role = dialog"
      if (GetAttribute(node, "aria-modal") == "true" &&
          GetAttribute(node, "role") == "dialog") {
        node = RemoveAndGetNext(node);
        continue;
      }

      // If we don't have a byline yet check to see if this node is a byline; if
      // it is store the byline and remove the node.
      if (articleByline.empty() && IsValidByLine(node, matchString)) {
        auto endOfSearchMarkerNode = GetNextNode(node, true);
        auto next = GetNextNode(node);
        lxb_dom_node_t *itempropNameNode = nullptr;
        while (next && next != endOfSearchMarkerNode) {
          auto itemprop = GetAttribute(next, "itemprop");
          if (itemprop.find("name") != std::string::npos) {
            itempropNameNode = next;
            break;
          } else {
            next = GetNextNode(next);
          }
        }

        auto source = itempropNameNode ? GetNodeTextContent(itempropNameNode)
                                       : GetNodeTextContent(node);
        Trim(source);
        articleByline = source;
        node = RemoveAndGetNext(node);
        continue;
      }

      if (shouldRemoveTitleHeader && HeaderDuplicateTitle(node)) {
        shouldRemoveTitleHeader = false;
        node = RemoveAndGetNext(node);
        continue;
      }

      if (stripUnlikelyCandidates) {
        if (std::regex_search(matchString,
                              REGEXPS.at(RegexPattern::UNLIKELY_CANDIDATES)) &&
            !std::regex_search(
                matchString,
                REGEXPS.at(RegexPattern::OK_MAYBE_ITS_A_CANDIDATE)) &&
            !HasAncestorTag(node, "TABLE") && !HasAncestorTag(node, "CODE") &&
            GetTagName(node) != "BODY" && GetTagName(node) != "A") {
          node = RemoveAndGetNext(node);
          continue;
        }

        if (UNLIKELY_ROLES.find(GetAttribute(node, "role")) !=
            UNLIKELY_ROLES.end()) {
          node = RemoveAndGetNext(node);
          continue;
        }
      }

      // Remove DIV, SECTION, and HEADER nodes without any content(e.g. text,
      // image, video, or iframe).
      auto tag = GetTagName(node);
      if ((tag == "DIV" || tag == "SECTION" || tag == "HEADER" || tag == "H1" ||
           tag == "H2" || tag == "H3" || tag == "H4" || tag == "H5" ||
           tag == "H6") &&
          IsElementWithoutContent(node)) {
        node = RemoveAndGetNext(node);
        continue;
      }
      if (DEFAULT_TAGS_TO_SCORE.find(GetTagName(node)) !=
          DEFAULT_TAGS_TO_SCORE.end()) {
        elementsToScore.push_back(node);
      }

      if (GetTagName(node) == "DIV") {
        auto childNode = GetFirstChild(node);
        while (childNode) {
          auto nextSibling = GetNextSibling(childNode);
          if (IsPhrasingContent(childNode)) {

            auto doc_fragment =
                lxb_dom_document_create_document_fragment(&doc->dom_document);
            auto fragment = lxb_dom_interface_node(doc_fragment);
            do {
              nextSibling = GetNextSibling(childNode);
              AppendChild(fragment, childNode);
              childNode = nextSibling;
            } while (childNode && IsPhrasingContent(childNode));

            // Trim leading and trailing whitespace from the fragment.
            auto fragmentFirstChild = GetFirstChild(fragment);
            while (fragmentFirstChild && IsWhitespace(fragmentFirstChild)) {
              RemoveNode(fragmentFirstChild);
              fragmentFirstChild = GetFirstChild(fragment);
            }
            auto fragmentLastChild = GetLastChild(fragment);
            while (fragmentLastChild && IsWhitespace(fragmentLastChild)) {
              RemoveNode(fragmentLastChild);
              fragmentLastChild = GetLastChild(fragment);
            }

            // If the fragment contains anything, wrap it in a paragraph and
            // insert it before the next non-phrasing node.

            if (GetFirstChild(fragment)) {
              auto p = CreateElement("P");
              AppendChild(p, fragment);
              InsertBefore(node, p, nextSibling);
            }
          }
          childNode = nextSibling;
        }
        // Sites like http://mobile.slate.com encloses each paragraph with a DIV
        // element. DIVs with only a P element inside and no text content can be
        // safely converted into plain P elements to avoid confusing the scoring
        // algorithm with DIVs with are, in practice, paragraphs.

        if (HasSingleTagInsideElement(node, "P") &&
            GetLinkDensity(node) < 0.25) {
          auto newNode = GetChildElements(node)[0];
          auto parentNode = GetParent(node);
          ReplaceChild(parentNode, newNode, node);
          node = newNode;
          elementsToScore.push_back(node);
        } else if (!HasChildBlockElement(node)) {
          node = SetNodeTag(node, "P");
          elementsToScore.push_back(node);
        }
      }
      node = GetNextNode(node);
    }
    /**
     * Loop through all paragraphs, and assign a score to them based on how
     * content-y they look. Then add their score to their parent node.
     *
     * A score is determined by things like number of commas, class names, etc.
     * Maybe eventually link density.
     **/
    std::vector<lxb_dom_node_t *> candidates;
    if(isDebugMode)
    {
        if (isDebugMode)
            LOG_DEBUG("scoring pass: elementsToScore.size()={} stripUnlikely={} weightClasses={} cleanConditionally={}",
                      elementsToScore.size(), stripUnlikelyCandidates,
                      IsFlagActive(FLAG_WEIGHT_CLASSES), IsFlagActive(FLAG_CLEAN_CONDITIONALLY));
    }
    for (auto elementToScore : elementsToScore) {
      if (!GetParent(elementToScore) ||
          GetTagName(GetParent(elementToScore)) == "") {
              if(isDebugMode)
                  LOG_DEBUG("skip (no parent) : {}", GetNodeInfo(elementToScore));
              continue;
      }
      auto innerText = GetInnerText(elementToScore);
      if (innerText.size() < 25) {
        if(isDebugMode)
            LOG_DEBUG("skip (short thext lenght) : {}", GetNodeInfo(elementToScore));
        continue;
      }
      auto ancestors = GetAncestors(elementToScore, 5);
      if (ancestors.size() == 0)
      {
          if(isDebugMode)
              LOG_DEBUG("skip (no ancestors): {}", GetNodeInfo(elementToScore));
          continue;
      }
      int contentScore = 0;

      // Add a point for the paragraph itself as a base.
      contentScore += 1;

      std::sregex_token_iterator it(innerText.begin(), innerText.end(),
                                    REGEXPS.at(RegexPattern::COMMAS), -1);
      std::sregex_token_iterator end;
      size_t count = 0;
      for (; it != end; ++it) {
        count++;
      }

      contentScore += count == 0 ? 1 : count;
      contentScore += std::min((int)innerText.size() / 100, 3);

      int scoreDivider;
      for (int level = 0; level < ancestors.size(); level++) {
        auto ancestor = ancestors[level];
        if (GetTagName(ancestor) == "" || !GetParent(ancestor) ||GetTagName(GetParent(ancestor)) == "")
        {

            continue;
        }

        if (!HasAttribute(ancestor, "readability")) {
          InitializeNode(ancestor);
          candidates.push_back(ancestor);
        }

        // Node score divider:
        // - parent:             1 (no division)
        // - grandparent:        2
        // - great grandparent+: ancestor level * 3
        if (level == 0) {
          scoreDivider = 1;
        } else if (level == 1) {
          scoreDivider = 2;
        } else
          scoreDivider = level * 3;

        auto readability = GetAttribute(ancestor, "readability");
        auto readabilityValue = std::stod(readability);
        readabilityValue += static_cast<double>(contentScore) / scoreDivider;
        SetAttribute(ancestor, "readability", std::to_string(readabilityValue));

      }
    }
    // After we've calculated scores, loop through all of the possible
    // candidate nodes we found and find the one with the highest score.
    std::vector<lxb_dom_node_t *> topCandidates;
    for (size_t c = 0, cl = candidates.size(); c < cl; c++) {
      auto candidate = candidates[c];
      // Scale the final candidates score based on link density. Good content
      // should have a relatively small link density (5% or less) and be mostly
      // unaffected by this operation.
      auto readability = GetAttribute(candidate, "readability");
      auto readabilityValue = std::stod(readability);
      auto candidateScore = readabilityValue * (1 - GetLinkDensity(candidate));
      SetAttribute(candidate, "readability", std::to_string(candidateScore));

      for (size_t t = 0; t < nbTopCandidates; t++) {
        lxb_dom_node_t *aTopCandidate = nullptr;
        if (t < topCandidates.size())
          aTopCandidate = topCandidates[t];

        double topCandidateScore = 0;
        if (aTopCandidate)
          topCandidateScore = std::stod(GetAttribute(aTopCandidate, "readability"));

        if (!aTopCandidate || candidateScore > topCandidateScore) {
          topCandidates.insert(topCandidates.begin() + t, candidate);
          if (topCandidates.size() > nbTopCandidates) {
            topCandidates.pop_back();
          }
          break;
        }
      }
    }
    lxb_dom_node_t *topCandidate = topCandidates.size() != 0 ? topCandidates[0] : nullptr;
    bool neededToCreateCandidate = false;
    lxb_dom_node_t *parentOfTopCandidate = nullptr;
    // If we still have no top candidate, just use the body as a last resort.
    // We also have to copy the body node so it is something we can modify.
    if (topCandidate == nullptr || GetTagName(topCandidate) == "BODY") {
      topCandidate = CreateElement("DIV");
      neededToCreateCandidate = true;
      while (GetFirstChild(page)) {
        AppendChild(topCandidate, GetFirstChild(page));
      }
      AppendChild(page, topCandidate);
      InitializeNode(topCandidate);
    } else if (topCandidate != nullptr) {
      // Find a better top candidate node if it contains (at least three) nodes
      // which belong to `topCandidates` array and whose scores are quite closed
      // with current `topCandidate` node
      std::vector<std::vector<lxb_dom_node_t *>> alternativeCandidateAncestors;
      for (size_t i = 1; i < topCandidates.size(); i++) {
        auto altTopCandidate = topCandidates[i];
        double altTopCandidateValue = std::stod(GetAttribute(altTopCandidate, "readability"));
        double topCandidateValue = std::stod(GetAttribute(topCandidate, "readability"));
        if (altTopCandidateValue / topCandidateValue >= 0.75)
        {
            alternativeCandidateAncestors.push_back(GetAncestors(altTopCandidate));
        }
      }
      size_t MINIMUM_TOPCANDIDATES = 3;
      if (alternativeCandidateAncestors.size() >= MINIMUM_TOPCANDIDATES) {
        parentOfTopCandidate = GetParent(topCandidate);
        while (GetTagName(parentOfTopCandidate) != "BODY") {
          int listsContainingTHisAncestor = 0;
          for (size_t ancestorIndex = 0;
               ancestorIndex < alternativeCandidateAncestors.size() &&
               listsContainingTHisAncestor < MINIMUM_TOPCANDIDATES;
               ancestorIndex++) {
            auto ancestor = alternativeCandidateAncestors[ancestorIndex];
            bool contains = std::find(ancestor.begin(), ancestor.end(),
                                      parentOfTopCandidate) != ancestor.end();
            listsContainingTHisAncestor += contains ? 1 : 0;
          }
          if (listsContainingTHisAncestor >= MINIMUM_TOPCANDIDATES) {
            topCandidate = parentOfTopCandidate;
            break;
          }
          parentOfTopCandidate = GetParent(parentOfTopCandidate);
        }
      }
      if (!HasAttribute(topCandidate, "readability")) {
        InitializeNode(topCandidate);
      }
      // Because of our bonus system, parents of candidates might have scores
      // themselves. They get half of the node. There won't be nodes with higher
      // scores than our topCandidate, but if we see the score going *up* in the
      // first few steps up the tree, that's a decent sign that there might be
      // more content lurking in other places that we want to unify in. The
      // sibling stuff below does some of that - but only if we've looked high
      // enough up the DOM tree.
      parentOfTopCandidate = GetParent(topCandidate);
      double latsScore = std::stod(GetAttribute(topCandidate, "readability"));
      double scoreThreshold = latsScore / 3;

      if(isDebugMode)
      {
          LOG_DEBUG("walk-up start: topCandidate = {} score = {} scoreThreshold = {}",
              GetNodeInfo(parentOfTopCandidate), latsScore, scoreThreshold);
      }

      while (parentOfTopCandidate &&
             GetTagName(parentOfTopCandidate) != "BODY") {
        if (!HasAttribute(parentOfTopCandidate, "readability")) {
          parentOfTopCandidate = GetParent(parentOfTopCandidate);
          continue;
        }
        double parentScore =
            std::stod(GetAttribute(parentOfTopCandidate, "readability"));
        if (parentScore < scoreThreshold) {
          break;
        }

        if (parentScore > latsScore) {
            if(isDebugMode)
            {
                LOG_DEBUG("walk-up: climbing to {} (parentScore={} > lastScore={})",
                            GetNodeInfo(parentOfTopCandidate), parentScore, latsScore);
            }
          topCandidate = parentOfTopCandidate;
          break;
        }
        latsScore = parentScore;
        parentOfTopCandidate = GetParent(parentOfTopCandidate);
      }
      // If the top candidate is the only child, use parent instead. This will
      // help sibling joining logic when adjacent content is actually located in
      // parent's sibling node.
      parentOfTopCandidate = GetParent(topCandidate);
      while (parentOfTopCandidate &&
             GetTagName(parentOfTopCandidate) != "BODY" &&
             GetChildElements(parentOfTopCandidate).size() == 1) {

        topCandidate = parentOfTopCandidate;
        parentOfTopCandidate = GetParent(topCandidate);
      }
      if (!HasAttribute(topCandidate, "readability")) {
        InitializeNode(topCandidate);
      }
    }

    // Now that we have the top candidate, look through its siblings for content
    // that might also be related. Things like preambles, content split by ads
    // that we removed, etc.
    auto articleContent = CreateElement("DIV");
    if (isPaging) {
      SetAttribute(articleContent, "id", "readability-content");
    }
    double siblingScoreThreshold = std::max(
        10.0, std::stod(GetAttribute(topCandidate, "readability")) * 0.2);
    parentOfTopCandidate = GetParent(topCandidate);
    auto siblings = GetChildElements(parentOfTopCandidate);
    for (size_t s = 0, sl = siblings.size(); s < sl; s++) {
      auto sibling = siblings[s];
      bool append = false;
      if (sibling == topCandidate) {
        append = true;
      } else {
        double contentBonus = 0;
        if (GetNodeClass(sibling) == GetNodeClass(topCandidate) &&
            GetNodeClass(topCandidate) != "") {
          contentBonus +=
              std::stod(GetAttribute(topCandidate, "readability")) * 0.2;
        }

        if (HasAttribute(sibling, "readability") &&
            std::stod(GetAttribute(sibling, "readability")) + contentBonus >=
                siblingScoreThreshold) {
          append = true;
        } else if (GetTagName(sibling) == "P") {
          auto linkDensity = GetLinkDensity(sibling);
          auto nodeContent = GetInnerText(sibling);
          auto nodeLength = nodeContent.size();

          if (nodeLength > 80 && linkDensity < 0.25) {
            append = true;
          } else if (nodeLength < 80 && nodeLength > 0 && linkDensity == 0 &&
                     std::regex_search(nodeContent, std::regex(R"(\.( |$))"))) {
            append = true;
          }
        }
      }
      if (append) {
        if (ALTER_TO_DIV_EXCEPTIONS.find(GetTagName(sibling)) ==
            ALTER_TO_DIV_EXCEPTIONS.end()) {
          sibling = SetNodeTag(sibling, "DIV");
        }
        AppendChild(articleContent, sibling);
        // Fetch children again to make it compatible
        // with DOM parsers without live collection support.
        siblings = GetChildElements(parentOfTopCandidate);
        // siblings is a reference to the children array, and
        // sibling is removed from the array when we call appendChild().
        // As a result, we must revisit this index since the nodes
        // have been shifted.
        s -= 1;
        sl -= 1;
      }
    }

    PrepArticle(articleContent);

    if (neededToCreateCandidate) {
      // We already created a fake div thing, and there wouldn't have been any
      // siblings left for the previous loop, so there's no point trying to
      // create a new div, and then move all the children over. Just assign
      // IDs and class names here. No need to append because that already
      // happened anyway.
      SetAttribute(topCandidate, "id", "readability-page-1");
      SetAttribute(topCandidate, "class", "page");
    } else {
      auto div = CreateElement("DIV");
      SetAttribute(div, "id", "readability-page-1");
      SetAttribute(div, "class", "page");
      while (GetFirstChild(articleContent)) {
        AppendChild(div, GetFirstChild(articleContent));
      }
      AppendChild(articleContent, div);
    }

    bool parseSuccessful = true;
    // Now that we've gone through the full algorithm, check to see if
    // we got any meaningful content. If we didn't, we may need to re-run
    // grabArticle with different flags set. This gives us a higher likelihood
    // of finding the content, and the sieve approach gives us a higher
    // likelihood of finding the -right- content.
    auto textLength = GetInnerText(articleContent, true).size();
    if (textLength < charThreshold_) {
      parseSuccessful = false;
      SetInnerHtml(page, pageCacheHtml);
      attempts_.push_back({articleContent, textLength});

      if (IsFlagActive(FLAG_STRIP_UNLIKELYS)) {
        RemoveFlag(FLAG_STRIP_UNLIKELYS);
      } else if (IsFlagActive(FLAG_WEIGHT_CLASSES)) {
        RemoveFlag(FLAG_WEIGHT_CLASSES);
      } else if (IsFlagActive(FLAG_CLEAN_CONDITIONALLY)) {
        RemoveFlag(FLAG_CLEAN_CONDITIONALLY);
      } else {
        // No luck after removing flags, just return the longest text we found
        // during the different loops
        std::sort(attempts_.begin(), attempts_.end(),
                  [](const Attempt &a, const Attempt &b) {
                    return b.textLength < a.textLength;
                  });
        if (attempts_.empty() || attempts_[0].textLength == 0) {
            if(isDebugMode)
                LOG_INFO("GrabArticle  returnd node: nullptr");
            return nullptr;
        }
        articleContent = attempts_[0].articleContent;
        parseSuccessful = true;
      }
    }

    if (parseSuccessful) {
      // Find out text direction from ancestors of final top candidate.
      std::vector<lxb_dom_node_t *> ancestors;
      ancestors.push_back(parentOfTopCandidate);
      ancestors.push_back(topCandidate);
      auto parentAncestors = GetAncestors(parentOfTopCandidate);
      ancestors.insert(ancestors.end(), parentAncestors.begin(),
                       parentAncestors.end());
      SomeNode(ancestors, [this](lxb_dom_node_t *ancestor) {
        if (!IsElementNode(ancestor)) {
          return false;
        }
        auto dir = GetAttribute(ancestor, "dir");
        if (!dir.empty()) {
          this->articleDir = dir;
          return true;
        }
        return false;
      });
      if(isDebugMode)
          LOG_INFO("GrabArticle returned node: {}", GetNodeInfo(articleContent));

      return lxb_dom_interface_element(articleContent);
    }
  }
}

void ArticleScraper::SetInnerHtml(lxb_dom_node_t *node, const std::string &html)
{
    if (!node || !IsElementNode(node))
        return;

    auto *html_elem = lxb_html_interface_element(lxb_dom_interface_element(node));
    lxb_html_element_inner_html_set(
        html_elem, reinterpret_cast<const lxb_char_t *>(html.data()),
        html.size());
}

void ArticleScraper::CleanStyles(lxb_dom_node_t *element)
{

    if(isDebugMode)
        LOG_INFO("CleanStyeles on node: {}", GetNodeInfo(element));

    if (element == nullptr || !IsElementNode(element))
        return;

    for (size_t i = 0; i < PRESENTATIONAL_ATTRIBUTES.size(); i++)
    {
        RemoveAttribute(element, PRESENTATIONAL_ATTRIBUTES.at(i));
    }

    if (DEPRECATED_SIZE_ATTRIBUTE_ELEMS.find(GetTagName(element)) !=
        DEPRECATED_SIZE_ATTRIBUTE_ELEMS.end())
    {
        RemoveAttribute(element, "width");
        RemoveAttribute(element, "height");
    }

    auto cur = GetFirstChildElement(element);
    while (cur != nullptr)
    {
        CleanStyles(cur);
        cur = GetNextElementSibling(cur);
    }
}

void ArticleScraper::PrepArticle(lxb_dom_node_t *articleContent)
{
    if(isDebugMode)
        LOG_INFO("preparing article : {}", GetNodeInfo(articleContent));

    CleanStyles(articleContent);
    MarkDataTables(articleContent);
    CleanConditionally(articleContent, "FORM");
    CleanConditionally(articleContent, "FIELDSET");
    Clean(articleContent, "OBJECT");
    Clean(articleContent, "EMBED");
    Clean(articleContent, "FOOTER");
    Clean(articleContent, "LINK");
    Clean(articleContent, "ASIDE");

    auto shareElementThreshold = charThreshold_;

    auto childrens = GetChildElements(articleContent);
    for (auto &child : childrens)
    {
        CleanMatchedNode(child, [this, shareElementThreshold](
                                    lxb_dom_node_t *node, std::string matchString) {
                                        return (GetNodeTextContent(node).size() < shareElementThreshold &&
                                            std::regex_search(matchString,
                                                REGEXPS.at(RegexPattern::SHARE_ELEMENTS)));
                                    });
    }

    Clean(articleContent, "IFRAME");
    Clean(articleContent, "INPUT");
    Clean(articleContent, "TEXTAREA");
    Clean(articleContent, "SELECT");
    Clean(articleContent, "BUTTON");
    CleanHeaders(articleContent);

    CleanConditionally(articleContent, "TABLE");
    CleanConditionally(articleContent, "UL");
    CleanConditionally(articleContent, "DIV");
    ReplaceNodeTags(GetAllNodesWithTag(articleContent, {"H1"}), "H2");

    // remove extra paragraphs
    RemoveNodes(
        GetAllNodesWithTag(articleContent, {"P"}), [this](lxb_dom_node_t *node) {
            auto contentElementCount =
                GetAllNodesWithTag(node, {"IMG", "EMBED", "OBJECT", "IFRAME"})
                    .size();
            return contentElementCount == 0 && GetInnerText(node, false).empty();
        });

    auto brNodes = GetAllNodesWithTag(articleContent, {"BR"});
    for (auto &br : brNodes)
    {
        auto next = GetNextNode(GetNextSibling(br));
        if (next && GetTagName(next) == "P")
        {
            RemoveNode(br);
        }
    }

    auto tableNodes = GetAllNodesWithTag(articleContent, {"TABLE"});
    for (auto &table : tableNodes)
    {
        auto tbody = HasSingleTagInsideElement(table, "TBODY")
                     ? GetFirstChildElement(table)
                     : table;
        if (HasSingleTagInsideElement(tbody, "TR"))
        {
            auto row = GetFirstChildElement(tbody);
            if (HasSingleTagInsideElement(row, "TD"))
            {
                auto cell = GetFirstChildElement(row);
                bool isAllPhrasingContent =
                    EveryNode(GetChildElements(cell), [this](lxb_dom_node_t *n) {
                        return IsPhrasingContent(n);
                    });
                auto newTag = isAllPhrasingContent ? "P" : "DIV";
                cell = SetNodeTag(cell, newTag);
                auto parent = GetParent(table);
                ReplaceChild(parent, cell, table);
            }
        }
    }
}

void ArticleScraper::CleanHeaders(lxb_dom_node_t *element)
{
    if(isDebugMode)
        LOG_INFO("cleaning header of: {}", GetNodeInfo(element));

    if (!IsElementNode(element))
        return;

    auto headingNodes = GetAllNodesWithTag(element, {"H1", "H2"});
    RemoveNodes(headingNodes, [this](lxb_dom_node_t *node) {
        return GetClassWeight(node) < 0;
    });
}

void ArticleScraper::RemoveNodes(std::vector<lxb_dom_node_t *> nodesToRemove, std::function<bool(lxb_dom_node_t *)> filter)
{
    for (int i = nodesToRemove.size() - 1; i >= 0; i--)
    {
        auto node = nodesToRemove[i];
        auto parentNode = GetParent(node);
        if (parentNode)
        {
            if (filter == nullptr || filter(node))
            {
                RemoveChilde(parentNode, node);
            }
        }
    }
}

void ArticleScraper::RemoveChilde(lxb_dom_node_t *parent, lxb_dom_node_t *child)
{

    if (parent == nullptr || child == nullptr)
        return;

    if(isDebugMode)
        LOG_INFO("removed child {} form parent", GetNodeInfo(child));

    auto err = lxb_dom_node_remove_child(parent, child);
    return;
}

void ArticleScraper::CleanMatchedNode(lxb_dom_node_t *element, std::function<bool(lxb_dom_node_t *, std::string)> filter)
{
    if (!IsElementNode(element))
        return;

    auto endOfSearchMarkerNode = GetNextNode(element, true);
    auto next = GetNextNode(element);
    while (next && next != endOfSearchMarkerNode)
    {
        if (filter(next, GetNodeClass(next) + " " + GetNodeId(next)))
        {
            next = RemoveAndGetNext(next);
        }
        else
        {
            next = GetNextNode(next);
        }
    }
}

void ArticleScraper::Clean(lxb_dom_node_t *element, std::string tag)
{
    std::unordered_set<std::string> embeds = {"OBJECT", "EMBED", "IFRAME"};
    auto isEmbed = embeds.find(tag) != embeds.end();
    auto nodesToRemove = GetElementsByTagName(element, tag);
    RemoveNodes(nodesToRemove, [this, isEmbed](lxb_dom_node_t *node)
        {
            if (isEmbed)
            {
                if (HasVideoAttribute(node))
                    return false;
            }
            return true;
        });
}



void ArticleScraper::CleanConditionally(lxb_dom_node_t *element, std::string tag)
{
    if (!IsFlagActive(FLAG_CLEAN_CONDITIONALLY))
    {
        return;
    }
    auto nodesToRemove = GetElementsByTagName(element, tag);
    RemoveNodes(nodesToRemove, [this, tag](lxb_dom_node_t *node) {
        auto isDataTable = [this](lxb_dom_node_t *node) {
            auto readabilityDataTable = GetAttribute(node, "readabilityDataTable");
            bool isDataTable = readabilityDataTable == "0" ? false : true;
            return isDataTable;
        };

    bool isList = tag == "UL" || tag == "OL";
    if (!isList)
    {
        int listLength = 0;
        auto listNodes = GetAllNodesWithTag(node, {"UL", "OL"});
        for (auto &listNode : listNodes)
        {
            listLength += GetInnerText(listNode).size();
        }
        isList = (double)listLength / (double)GetInnerText(node).size() > 0.9;
    }

    if (tag == "TABLE" && isDataTable(node))
    {
        return false;
    }

    if (HasAncestorTag(node, "TABLE", -1, isDataTable))
    {
        return false;
    }

    if (HasAncestorTag(node, "CODE"))
    {
        return false;
    }

    if (SomeNode(GetElementsByTagName(node, "TABLE"),
                 [this](lxb_dom_node_t *node_) {
                     if(!HasAttribute(node_, "readabilityDataTable"))
                     {
                         return false;
                     }
                     auto redabilityValue = GetAttribute(node_, "readabilityDataTable");


                   return redabilityValue == "1";
                 })) {
      return false;
    }

    auto weight = GetClassWeight(node);
    int contentScore = 0;
    if (weight + contentScore < 0) {
      return true;
    }

    if (GetCharCount(node, ',') < 10) {
      // If there are not very many commas, and the number of
      // non-paragraph elements is more than paragraphs or other
      // ominous signs, remove the element.
      auto p = GetElementsByTagName(node, "P").size();
      auto img = GetElementsByTagName(node, "IMG").size();
      auto li = (int)GetElementsByTagName(node, "LI").size() - 100;
      auto input = GetElementsByTagName(node, "INPUT").size();
      auto headingDensity = GetTextDensity(node, {
                                                     "H1",
                                                     "H2",
                                                     "H3",
                                                     "H4",
                                                     "H5",
                                                     "H6",
                                                 });
      int embedCount = 0;
      auto embeds = GetAllNodesWithTag(node, {"OBJECT", "EMBED", "IFRAME"});

      for (size_t i = 0; i < embeds.size(); i++) {
        if (HasVideoAttribute(embeds[i])) {
          return false;
        }
        embedCount++;
      }

      auto innerText = GetInnerText(node);
      if (std::regex_search(innerText, REGEXPS.at(RegexPattern::AD_WORDS)) ||
          std::regex_search(innerText,
                            REGEXPS.at(RegexPattern::LOADING_WORDS))) {
        return true;
      }

      auto contentLength = innerText.size();
      auto linkDensity = GetLinkDensity(node);
      std::vector<std::string> textishTags = {"SPAN", "LI", "TD"};
      textishTags.insert(textishTags.end(), DIV_TO_P_ELEMS.begin(),
                         DIV_TO_P_ELEMS.end());
      auto textDensity = GetTextDensity(node, textishTags);
      auto isFIgureChild = HasAncestorTag(node, "FIGURE");
      auto shouldRemoveNode = [&]() {
        if (!isFIgureChild && img > 1 && (double)p / (double)img < 0.5) {
          return true;
        }

        if (!isList && li > p) {
          return true;
        }

        if (input > p / 3)
          return true;

        if (!isList && !isFIgureChild && headingDensity < 0.9 &&
            contentLength < 25 && (img == 0 || img > 2) && linkDensity > 0) {
          return true;
        }

        // TODO: consider adding link density modifier
        if (!isList && weight < 25 && linkDensity > 0.2)
          return true;

        // TODO: consider adding link density modifier
        if (weight >= 25 && linkDensity > 0.5)
          return true;

        if ((embedCount == 1 && contentLength < 75) || embedCount > 1)
          return true;

        if (img == 0 && textDensity == 0)
          return true;

        return false;
      };

      auto haveToRemove = shouldRemoveNode();
      if (isList && haveToRemove) {
        auto childrens = GetChildElements(node);
        for (size_t c = 0; c < childrens.size(); c++) {
          auto child = childrens[c];
          if (GetChildElementCount(child) > 1) {
            return haveToRemove;
          }
        }

        auto li_count = GetElementsByTagName(node, "LI").size();
        if (img == li_count)
          return false;
      }
      return haveToRemove;
    }
    return false;
  });
}

bool ArticleScraper::HasVideoAttribute(lxb_dom_node_t *node) {
  if (!IsElementNode(node))
    return false;

  auto *element = lxb_dom_interface_element(node);
  for (auto *attr = lxb_dom_element_first_attribute(element); attr != nullptr;
       attr = lxb_dom_element_next_attribute(attr)) {
    size_t len = 0;
    const lxb_char_t *v = lxb_dom_attr_value(attr, &len);
    if (v == nullptr || len == 0)
      continue;

    std::string value(reinterpret_cast<const char *>(v), len);
    if (std::regex_search(value, REGEXPS.at(RegexPattern::VIDEOS))) {
      return true;
    }
  }

  if (GetTagName(node) == "OBJECT" &&
      std::regex_search(GetInnerHtml(node), REGEXPS.at(RegexPattern::VIDEOS))) {
    return true;
  }

  return false;
}

double ArticleScraper::TextSimilarity(std::string textA, std::string textB)
{
    auto tokenize = [this](std::string s) {
        std::vector<std::string> tokens;
        auto pattern = this->REGEXPS.at(RegexPattern::TOKENIZE);
        std::transform(s.begin(), s.end(), s.begin(),
                    [](unsigned char c) { return std::tolower(c); });
        std::sregex_token_iterator it(s.begin(), s.end(), pattern, -1);
        std::sregex_token_iterator end;

        for (; it != end; ++it)
        {
            std::string token = *it;
            if (!token.empty()) { // equivalent of filter(Boolean)
                tokens.push_back(token);
            }
        }

        return tokens;
    };

    auto tokensA = tokenize(textA);
    auto tokensB = tokenize(textB);
    if (!tokensA.size() || !tokensB.size())
        return 0.0;

    std::vector<std::string> uniqTOkensB;
    for (auto token : tokensB)
    {
        auto found = std::find(tokensA.begin(), tokensA.end(), token) != tokensA.end();
        if (!found) {
            uniqTOkensB.push_back(token);
        }
    }
    auto join = [](std::vector<std::string> tokens) {
        std::string joinedString = "";
        for (int i = 0; i < tokens.size(); i++) {
            if (i > 0)
                joinedString += " ";
            joinedString += tokens[i];
    }
        return joinedString;
    };
    double distanceB = (double)join(uniqTOkensB).size() /   (double)join(tokensB).size();
    return 1 - distanceB;
}

bool ArticleScraper::HeaderDuplicateTitle(lxb_dom_node_t *element)
{
    auto nodeTag = GetTagName(element);
    if (nodeTag != "H1" && nodeTag != "H2")
        return false;

    auto heading = GetInnerText(element, false);

    return TextSimilarity(this->articleTitle, heading) > 0.75;
}

void ArticleScraper::RemoveScripts()
{
    auto script_nodes = GetAllNodesWithTag(root, {"SCRIPT", "NOSCRIPT"});
    RemoveNodes(script_nodes, nullptr);
}

lxb_dom_node_t *ArticleScraper::GetLastChild(lxb_dom_node_t *node)
{
    if (node == nullptr)
        return nullptr;
    return node->last_child;
}

/**
 * Replaces 2 or more successive <br> elements with a single <p>.
 * Whitespace between <br> elements are ignored. For example:
 *   <div>foo<br>bar<br> <br><br>abc</div>
 * will become:
 *   <div>foo<br>bar<p>abc</p></div>
 */
void ArticleScraper::ReplaceBrs(lxb_dom_node_t *element) {
  if (element == nullptr)
    return;

  auto br_nodes = GetAllNodesWithTag(element, {"BR"});
  for (auto br_node : br_nodes) {
    auto next = GetNextSibling(br_node);

    // Whether 2 or more <br> elements have been found and replaced with a
    // <p> block.
    bool replaced = false;

    // If we find a <br> chain, remove the <br>s until we hit another node
    // or non-whitespace. This leaves behind the first <br> in the chain
    // (which will be replaced with a <p> later).
    while ((next = GetNextNode(next)) && GetTagName(next) == "BR") {
      replaced = true;
      auto brSibling = GetNextSibling(next);
      RemoveNode(next);
      next = brSibling;
    }

    if (replaced) {
      auto p = CreateElement("P");
      if (p != nullptr) {
        auto parent = GetParent(br_node);
        ReplaceChild(parent, p, br_node);
        next = GetNextSibling(p);

        while (next != nullptr) {
          if (GetTagName(next) == "BR") {
            auto nextElem = GetNextNode(GetNextSibling(next));
            if (nextElem != nullptr && GetTagName(nextElem) == "BR") {
              break;
            }
          }

          if (!IsPhrasingContent(next)) {
            break;
          }

          auto sibling = GetNextSibling(next);
          AppendChild(p, next);
          next = sibling;
        }

        while (GetLastChild(p) != nullptr && IsWhitespace(GetLastChild(p))) {
          RemoveNode(GetLastChild(p));
        }

        if (GetTagName(GetParent(p)) == "P") {
          SetNodeTag(GetParent(p), "DIV");
        }
      }
    }
  }
}

void ArticleScraper::ReplaceNodeTags(std::vector<lxb_dom_node_t *> nodes, const std::string &newTag)
{
    for (auto node : nodes)
    {
        SetNodeTag(node, newTag);
    }
}

void ArticleScraper::PrepDocument()
{
    if(isDebugMode)
        LOG_INFO("starting prepering document");

    auto doc = this->doc;
    auto style_nodes = GetAllNodesWithTag(root, {"STYLE"});
    RemoveNodes(style_nodes, nullptr);
    if (doc->body)
    {
        ReplaceBrs(dom);
    }

    // this._replaceNodeTags(this._getAllNodesWithTag(doc, ["font"]), "SPAN");
    auto font_nodes = GetAllNodesWithTag(root, {"FONT"});
    ReplaceNodeTags(font_nodes, "SPAN");

    if(isDebugMode)
        LOG_INFO("finished preparing document");
}

void ArticleScraper::ParseHTML(std::string html_page) {

  if(isDebugMode)
      LOG_INFO("html parsing started");
  doc = lxb_html_document_create();
  if (doc == nullptr) {
      FreeResources();
      throw HTMLParsingException(
          "couldn't allocate enough space during parsing creation");
  }

  auto code = lxb_html_document_parse(doc, (const lxb_char_t *)html_page.data(),
                                      html_page.size());
  if (code != LXB_STATUS_OK) {
    FreeResources();
    throw HTMLParsingException("couldn't parse the html page");
  }

  dom = lxb_dom_interface_node(lxb_html_document_body_element(doc));
  root = lxb_dom_interface_node(doc->dom_document.element);

  if(isDebugMode)
      LOG_INFO("html parsing finished");
}

void ArticleScraper::FreeResources()
{
    if(isDebugMode)
        LOG_INFO("freeing resources");

    if (doc != nullptr)
    {
        lxb_html_document_destroy(doc);
        doc = nullptr;
        dom = nullptr;
        root = nullptr;
    }
}

/**
 * Find all <noscript> that are located after <img> nodes, and which contain
 * only one <img> element. Replace the first image with the image from inside
 * the <noscript> tag, and remove the <noscript> tag. This improves the quality
 * of the images we use on some sites (e.g. Medium).
 *
 *
 **/
void ArticleScraper::UnwrapNoScriptImages() {}

ArticleScraper::~ArticleScraper() { FreeResources(); }

void ArticleScraper::MarkDataTables(lxb_dom_node_t *root)
{
  if(isDebugMode)
      LOG_INFO("MArking data table from root: {}", GetNodeInfo(root));

  auto tables = GetElementsByTagName(root, "TABLE");
  for (size_t i = 0; i < tables.size(); i++) {
    auto table = tables[i];
    auto role = GetAttribute(table, "role");
    if (role == "presentation") {
      SetAttribute(table, "readabilityDataTable", "0");
      continue;
    }

    auto datatable = GetAttribute(table, "datatable");
    if (datatable == "0") {
      SetAttribute(table, "readabilityDataTable", "0");
      continue;
    }

    auto summary = GetAttribute(table, "summary");
    if (summary != "") {

      SetAttribute(table, "readabilityDataTable", "1");
      continue;
    }

    auto captions = GetElementsByTagName(table, "CAPTION");
    if (captions.size() > 0 && GetChildElementCount(captions[0])) {

      SetAttribute(table, "readabilityDataTable", "1");
      continue;
    }

    std::vector<std::string> dataTableDescendants = {"COL", "COLGROUP", "TFOOT",
                                                     "THEAD", "TH"};
    auto descendantExists = [this, table](std::string tag) {
      auto elements = GetElementsByTagName(table, tag);
      return elements.size() > 0;
    };
    bool some = false;
    for (auto &tableDescendant : dataTableDescendants) {
      if (descendantExists(tableDescendant)) {

        some = true;
        break;
      }
    }
    if (some) {
      SetAttribute(table, "readabilityDataTable", "1");
      continue;
    }

    if (GetElementsByTagName(table, "TABLE").size() > 0) {

      SetAttribute(table, "readabilityDataTable", "0");
      continue;
    }

    int rows = 0;
    int columns = 0;
    auto trs = GetElementsByTagName(table, "TR");
    if (trs.empty()) {
      SetAttribute(table, "readabilityDataTable", "0");
      continue;
    }
    for (auto &tr : trs) {
      std::string rs = GetAttribute(tr, "rowspan"); // "" if missing
      int rowspan = 0;
      if (!rs.empty()) {
        try {
          rowspan = std::stod(rs);
        } // like parseInt: "3abc"->3
        catch (...) {
          rowspan = 0;
        } // like NaN -> falsy
      }
      rows += (rowspan != 0 ? rowspan : 1);

      int columnsInThisRow = 0;
      for (auto &cell : GetElementsByTagName(tr, "TD")) {
        std::string cs = GetAttribute(cell, "colspan"); // "" if missing
        int colspan = 0;
        if (!cs.empty()) {
          try {
            colspan = std::stod(cs);
          } // like parseInt: "3abc"->3
          catch (...) {
            colspan = 0;
          } // like NaN -> falsy
        }
        columnsInThisRow += (colspan != 0 ? colspan : 1);
      }
      columns = std::max(columns, columnsInThisRow);
    }

    if (columns == 1 || rows == 1) {
      SetAttribute(table, "readabilityDataTable", "0");
      continue;
    }

    if (rows >= 10 || columns > 4) {
      SetAttribute(table, "readabilityDataTable", "1");
      continue;
    }

    auto value = rows * columns > 10 ? "1" : "0";
    SetAttribute(table, "readabilityDataTable", value);
  }
}

std::vector<lxb_dom_node_t *> ArticleScraper::GetElementsByTagName(lxb_dom_node_t *root, const std::string &tag_name)
{
  std::vector<lxb_dom_node_t *> nodes;
  if(root == nullptr)
      return nodes;
  auto upperTag = ToUpper(tag_name);
  auto endMarker = GetNextNode(root, true);
  lxb_dom_node_t *current = GetFirstChildElement(root);
  while (current != nullptr && current != endMarker) {
    if (GetTagName(current) == upperTag) {
      nodes.push_back(current);
    }
    current = GetNextNode(current);
  }
  return nodes;
}

std::vector<lxb_dom_node_t *> ArticleScraper::GetAllNodesWithTag(lxb_dom_node_t *root, std::unordered_set<std::string> tag_names)
{
  std::unordered_set<std::string> upperTags;
  for (const auto &tag : tag_names) {
    upperTags.insert(ToUpper(tag));
  }
  std::vector<lxb_dom_node_t *> nodes;
  if(root == nullptr)
      return nodes;
  auto endMarker = GetNextNode(root, true);
  lxb_dom_node_t *current = GetFirstChild(root);
  while (current != nullptr && current != endMarker) {
    if (upperTags.find(GetTagName(current)) != upperTags.end()) {
      nodes.push_back(current);
    }
    current = GetNextNode(current);
  }
  return nodes;
}

lxb_dom_node_t *ArticleScraper::GetParent(lxb_dom_node_t *node)
{
  if (node == nullptr)
    return nullptr;
  return node->parent;
}

std::vector<lxb_dom_node_t *> ArticleScraper::GetChildElements(lxb_dom_node_t *node)
{
  std::vector<lxb_dom_node_t *> elements;
  lxb_dom_node_t *current = GetFirstChild(node);
  while (current != nullptr) {
    if (IsElementNode(current)) {
      elements.push_back(current);
    }
    current = GetNextSibling(current);
  }
  return elements;
}

std::string ArticleScraper::GetNodeTextContent(lxb_dom_node_t *node, bool trim) {
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

bool ArticleScraper::IsElementNode(lxb_dom_node_t *node) {
  if (node == nullptr)
    return false;

  return node->type == LXB_DOM_NODE_TYPE_ELEMENT;
}

bool ArticleScraper::IsTextNode(lxb_dom_node_t *node) {
  if (node == nullptr)
    return false;

  return node->type == LXB_DOM_NODE_TYPE_TEXT;
}

std::string ArticleScraper::GetTagName(lxb_dom_node_t *node) {
  if (node == nullptr || !IsElementNode(node)) {
    return "";
  }

  size_t len = 0;
  const lxb_char_t *name = lxb_dom_element_qualified_name_upper(
      lxb_dom_interface_element(node), &len);
  std::string result(reinterpret_cast<const char *>(name), len);

  return result;
}

std::string ArticleScraper::GetNodeId(lxb_dom_node_t *node) {
  if (node == nullptr || !IsElementNode(node)) {
    return "";
  }

  size_t len = 0;
  const lxb_char_t *id =
      lxb_dom_element_id(lxb_dom_interface_element(node), &len);
  if (id == nullptr)
    return "";

  std::string result(reinterpret_cast<const char *>(id), len);

  return result;
}

std::string ArticleScraper::GetNodeClass(lxb_dom_node_t *node) {
  if (node == nullptr || !IsElementNode(node)) {
    return "";
  }

  size_t len = 0;
  const lxb_char_t *cls =
      lxb_dom_element_class(lxb_dom_interface_element(node), &len);
  std::string result(reinterpret_cast<const char *>(cls), len);

  return result;
}

std::string ArticleScraper::GetAttribute(lxb_dom_node_t *node, const std::string &attr_name) {
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

bool ArticleScraper::HasAttribute(lxb_dom_node_t *node,
                                  const std::string &attr_name) {
  if (node == nullptr || !IsElementNode(node)) {
    return false;
  }

  size_t len = 0;
  const lxb_char_t *attr =
      lxb_dom_element_get_attribute(lxb_dom_interface_element(node),
                                    reinterpret_cast<const lxb_char_t *>(
                                        const_cast<char *>(attr_name.c_str())),
                                    attr_name.size(), &len);
  return attr != nullptr;
}

void ArticleScraper::SetAttribute(lxb_dom_node_t *node, const std::string &attr_name, const std::string &value)
{
  if (node == nullptr || !IsElementNode(node)) {
    return;
  }

  if(isDebugMode)
      LOG_INFO("Setting attribute {} with value {} on node {}", attr_name, value, GetNodeInfo(node));

  lxb_dom_element_set_attribute(
      lxb_dom_interface_element(node),
      reinterpret_cast<const lxb_char_t *>(
          const_cast<char *>(attr_name.c_str())),
      attr_name.size(),
      reinterpret_cast<const lxb_char_t *>(const_cast<char *>(value.c_str())),
      value.size());
}

void ArticleScraper::RemoveAttribute(lxb_dom_node_t *node, const std::string &attr_name)
{
  if (node == nullptr || !IsElementNode(node)) {
    return;
  }

  if(isDebugMode)
      LOG_INFO("removing attribute {} from node {}", attr_name, GetNodeInfo(node));

  lxb_dom_element_remove_attribute(lxb_dom_interface_element(node),
                                   reinterpret_cast<const lxb_char_t *>(
                                       const_cast<char *>(attr_name.c_str())),
                                   attr_name.size());
}

lxb_dom_node_t *ArticleScraper::GetFirstChild(lxb_dom_node_t *node)
{
  if (node == nullptr)
    return nullptr;
  return node->first_child;
}

lxb_dom_node_t *ArticleScraper::GetNextSibling(lxb_dom_node_t *node)
{
  if (node == nullptr)
    return nullptr;

  return node->next;
}

lxb_dom_node_t *ArticleScraper::GetPrevSibling(lxb_dom_node_t *node) {
  if (node == nullptr)
    return nullptr;

  return node->prev;
}

lxb_dom_node_t *ArticleScraper::GetFirstChildElement(lxb_dom_node_t *node) {
  if (node == nullptr) {
    return nullptr;
  }

  lxb_dom_node_t *child = node->first_child;
  while (child != nullptr) {
    if (IsElementNode(child)) {
      return child;
    }
    child = child->next;
  }

  return nullptr;
}

// Get next ELEMENT sibling (skips text/comment nodes)
lxb_dom_node_t *ArticleScraper::GetNextElementSibling(lxb_dom_node_t *node) {
  if (node == nullptr)
    return nullptr;

  lxb_dom_node_t *sibling = node->next;
  while (sibling != nullptr) {
    if (IsElementNode(sibling)) {
      return sibling;
    }
    sibling = sibling->next;
  }

  return nullptr;
}

// Get previous ELEMENT sibling
lxb_dom_node_t *ArticleScraper::GetPrevElementSibling(lxb_dom_node_t *node) {
  if (node == nullptr)
    return nullptr;

  lxb_dom_node_t *sibling = node->prev;
  while (sibling != nullptr) {
    if (IsElementNode(sibling)) {
      return sibling;
    }
    sibling = sibling->prev;
  }

  return nullptr;
}

// Count element children
size_t ArticleScraper::GetChildElementCount(lxb_dom_node_t *node) {
  if (node == nullptr) {
    return 0;
  }

  size_t count = 0;
  lxb_dom_node_t *child = node->first_child;
  while (child != nullptr) {
    if (IsElementNode(child)) {
      count++;
    }
    child = child->next;
  }

  return count;
}

// Depth-first traversal (skip self if ignoreSelfAndKids=true)
lxb_dom_node_t *ArticleScraper::GetNextNode(lxb_dom_node_t *node, bool ignoreSelfAndKids) {
  if (node == nullptr)
    return nullptr;

  if (!ignoreSelfAndKids && GetFirstChildElement(node))
    return GetFirstChildElement(node);

  if (GetNextSibling(node))
    return GetNextSibling(node);

  // move up the parent chain *and* find a sibling
  // (because this is depth-first traversal, we will have already
  // seen the parent nodes themselves).
  do {
    node = GetParent(node);
  } while (node && !GetNextElementSibling(node));

  return node ? GetNextElementSibling(node) : nullptr;
}

// Get ancestor chain (up to maxDepth, 0 = unlimited)
std::vector<lxb_dom_node_t *> ArticleScraper::GetAncestors(lxb_dom_node_t *node, size_t maxDepth)
{
  std::vector<lxb_dom_node_t *> ancestors;
  size_t depth = 0;

  while (GetParent(node)) {
    ancestors.push_back(GetParent(node));
    if (++depth == maxDepth)
      break;
    node = GetParent(node);
  }
  return ancestors;
}

// Remove node from tree
void ArticleScraper::RemoveNode(lxb_dom_node_t *node) {
  if (node == nullptr)
    return;

  if(isDebugMode)
      LOG_INFO("Removing node: {}", GetNodeInfo(node));

  lxb_dom_node_remove(node);
}

// Remove and return the next node (for safe iteration during removal)
lxb_dom_node_t *ArticleScraper::RemoveAndGetNext(lxb_dom_node_t *node) {
  if (node == nullptr)
    return nullptr;

  auto next_node = GetNextNode(node, true);
  RemoveNode(node);
  return next_node;
}

// Replace child: parent replaces oldChild with newChild
void ArticleScraper::ReplaceChild(lxb_dom_node_t *parent,
                                  lxb_dom_node_t *newChild,
                                  lxb_dom_node_t *oldChild) {
  if (parent == nullptr || newChild == nullptr || oldChild == nullptr)
    return;

  lxb_dom_node_insert_before(oldChild, newChild);
  lxb_dom_node_remove(oldChild);
  // lxb_dom_node_destroy(oldChild); // actually frees it
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

  auto element = lxb_dom_document_create_element(
      &doc->dom_document, reinterpret_cast<const lxb_char_t *>(tagName.c_str()),
      tagName.size(), nullptr);

  if (element == nullptr)
    return nullptr;

  return lxb_dom_interface_node(element);
}

// Create text node
lxb_dom_node_t *ArticleScraper::CreateTextNode(const std::string &text)
{
  auto text_node = lxb_dom_document_create_text_node(
      &doc->dom_document, reinterpret_cast<const lxb_char_t *>(text.c_str()),
      text.size());
  if (text_node == nullptr)
    return nullptr;
  return lxb_dom_interface_node(text_node);
}

// Change a node's tag (create new element, move children + attrs, replace in
// tree)
lxb_dom_node_t *ArticleScraper::SetNodeTag(lxb_dom_node_t *node, const std::string &newTag)
{
    if (node == nullptr)
        return nullptr;
    auto replacement = CreateElement(newTag);

    if (replacement == nullptr)
        return node;

  while (GetFirstChild(node)) {
    auto child = GetFirstChild(node);
    AppendChild(replacement, child);
  }

  auto parent = GetParent(node);
  ReplaceChild(parent, replacement, node);

  if (HasAttribute(node, "readability")) {
    auto readability_value = GetAttribute(node, "readability");
    SetAttribute(replacement, "readability", readability_value);
  }

  if (IsElementNode(node)) {
    auto old_element = lxb_dom_interface_element(node);
    auto new_element = lxb_dom_interface_element(replacement);

    auto attr = lxb_dom_element_first_attribute(old_element);
    while (attr != nullptr) {
      size_t name_len = 0;
      size_t value_len = 0;
      const auto name = lxb_dom_attr_qualified_name(attr, &name_len);
      const auto value = lxb_dom_attr_value(attr, &value_len);

      if (name != nullptr) {
        std::string name_str(reinterpret_cast<const char *>(name), name_len);
        std::string value_str(reinterpret_cast<const char *>(value), value_len);
        SetAttribute(replacement, name_str, value_str);
      }
      attr = lxb_dom_element_next_attribute(attr);
    }
  }

  return replacement;
}

// Get inner text of a node (all text descendants, whitespace normalized
// optionally)
std::string ArticleScraper::GetInnerText(lxb_dom_node_t *node, bool normalize) {
  if (node == nullptr)
    throw std::invalid_argument("Node is null");

  if (!IsElementNode(node))
    throw std::invalid_argument("Node is not an element node");

  auto textContent = GetNodeTextContent(node);
  Trim(textContent);

  if (normalize) {
    auto pattern = REGEXPS.at(RegexPattern::NORMALIZE);
    textContent = std::regex_replace(textContent, pattern, " ");
  }

  return textContent;
}

void ArticleScraper::Trim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

std::string ArticleScraper::ToUpper(const std::string &s) {
  std::string out = s;
  std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
    return std::toupper(c);
  });
  return out;
}

// Get link density: ratio of text inside <a> tags vs total text
float ArticleScraper::GetLinkDensity(lxb_dom_node_t *node) {
  float textLength = GetInnerText(node).size();
  if (textLength == 0)
    return 0;
  float linkLength = 0;

  auto elements = GetElementsByTagName(node, "A");
  for (auto linknode : elements) {
    auto href = GetAttribute(linknode, "href");
    float coefficient =
        (href != "") &&
                std::regex_search(href, REGEXPS.at(RegexPattern::HASH_URL))
            ? 0.3f
            : 1.0f;
    linkLength += GetInnerText(linknode).size() * coefficient;
  }

  return linkLength / textLength;
}

// Get class weight: +/-25 per positive/negative regex match on class+id
int ArticleScraper::GetClassWeight(lxb_dom_node_t *node) {
  if (!IsFlagActive(FLAG_WEIGHT_CLASSES)) {
    return 0;
  }
  int weight = 0;
  std::string className = GetNodeClass(node);
  std::string idName = GetNodeId(node);

  if (!className.empty()) {
    if (std::regex_search(className, REGEXPS.at(RegexPattern::POSITIVE)))
      weight += 25;
    if (std::regex_search(className, REGEXPS.at(RegexPattern::NEGATIVE)))
      weight -= 25;
  }

  if (!idName.empty()) {
    if (std::regex_search(idName, REGEXPS.at(RegexPattern::POSITIVE)))
      weight += 25;
    if (std::regex_search(idName, REGEXPS.at(RegexPattern::NEGATIVE)))
      weight -= 25;
  }

  return weight;
}
// Count occurrences of a character in node's text
int ArticleScraper::GetCharCount(lxb_dom_node_t *node, char delimiter)
{
    auto text = GetInnerText(node);
    return std::count(text.begin(), text.end(), delimiter);
}

// Get text density: ratio of text in specified child tags vs total
float ArticleScraper::GetTextDensity(lxb_dom_node_t *node, const std::vector<std::string> &tags)
{
  float totalTextLength = GetInnerText(node).size();
  if (totalTextLength == 0)
    return 0;

  float childTextLength = 0;
  for (const auto &tag : tags) {
    auto elements = GetElementsByTagName(node, tag);
    for (auto childNode : elements) {
      childTextLength += GetInnerText(childNode).size();
    }
  }

  return childTextLength / totalTextLength;
}

bool ArticleScraper::EveryNode(std::vector<lxb_dom_node_t *> nodes, std::function<bool(lxb_dom_node_t *)> predicate)
{
  for (auto node : nodes) {
    if (!predicate(node))
      return false;
  }
  return true;
}

// Is this node phrasing content? (inline elements, text nodes, etc.)
bool ArticleScraper::IsPhrasingContent(lxb_dom_node_t *node)
{
  if (node == nullptr)
    return false;

  return IsTextNode(node) ||
         PHRASING_ELEMS.find(GetTagName(node)) != PHRASING_ELEMS.end() ||
         ((GetTagName(node) == "A" || GetTagName(node) == "DEL" ||
           GetTagName(node) == "INS") &&
          EveryNode(GetChildElements(node), [this](lxb_dom_node_t *child) {
            return IsPhrasingContent(child);
          }));
}

// Is this node whitespace? (empty text node or <br>)
bool ArticleScraper::IsWhitespace(lxb_dom_node_t *node) {
  if (node == nullptr)
    return false;

  if (IsTextNode(node)) {
    std::string text = GetNodeTextContent(node);
    Trim(text);
    return text.empty();
  }

  if (IsElementNode(node) && GetTagName(node) == "BR")
    return true;

  return false;
}

// Is this element empty? (no text, no meaningful children)
bool ArticleScraper::IsElementWithoutContent(lxb_dom_node_t *node) {
  if (IsElementNode(node)) {
    std::string text = GetInnerText(node);
    Trim(text);

    if (!text.empty())
      return false;

    auto children = GetChildElements(node);
    return children.empty() ||
           children.size() == GetElementsByTagName(node, "BR").size() +
                                  GetElementsByTagName(node, "HR").size();
  }

  return false;
}

bool ArticleScraper::SomeNode(std::vector<lxb_dom_node_t *> nodes,
                              std::function<bool(lxb_dom_node_t *)> predicate) {
  for (auto node : nodes) {
    if (predicate(node))
      return true;
  }
  return false;
}

// Does this element have any block-level child elements?
bool ArticleScraper::HasChildBlockElement(lxb_dom_node_t *element) {

  if (!IsElementNode(element))
    return false;

  return SomeNode(GetChildElements(element), [this](lxb_dom_node_t *child) {
    return DIV_TO_P_ELEMS.find(GetTagName(child)) != DIV_TO_P_ELEMS.end() ||
           HasChildBlockElement(child);
  });
}

// Does this element have exactly one child of given tag, with no real text?
bool ArticleScraper::HasSingleTagInsideElement(lxb_dom_node_t *element, const std::string &tag)
{

  if (element == nullptr)
    return false;

  if (!IsElementNode(element))
      return false;

  auto children = GetChildElements(element);
  if (children.size() != 1)
    return false;

  auto child = children[0];
  if (GetTagName(child) != ToUpper(tag))
    return false;

  return !SomeNode(GetChildElements(child), [this](lxb_dom_node_t *node) {
    return IsTextNode(node) &&
           std::regex_search(GetNodeTextContent(node),
                            REGEXPS.at(RegexPattern::HAS_CONTENT));
  });
}

// Does this node have an ancestor with the given tag name (up to maxDepth)?
bool ArticleScraper::HasAncestorTag(lxb_dom_node_t *node, const std::string &tagName, size_t maxDepth, std::function<bool(lxb_dom_node_t *)> filter) {
  int depth = 0;
  while (GetParent(node)) {
    if (maxDepth > 0 && depth > maxDepth)
      return false;

    if (GetTagName(GetParent(node)) == ToUpper(tagName) &&
        (!filter || filter(GetParent(node))))
      return true;

    node = GetParent(node);
    depth++;
  }

    return false;
}

std::string ArticleScraper::GetStyleAttribute(lxb_dom_node_t *node,
                                              const std::string &property) {
  if (node == nullptr || !IsElementNode(node))
    return "";

  auto style = GetAttribute(node, "style");
  if (style.empty())
    return "";

  std::regex pattern(property + R"(\s*:\s*([^;]+))",
                     std::regex_constants::icase);
  std::smatch match;
  if (std::regex_search(style, match, pattern)) {
    return match[1].str();
  }

  return "";
}

// Is this node probably visible? (not display:none, not hidden, etc.)
bool ArticleScraper::IsProbablyVisible(lxb_dom_node_t *node) {

    auto display = GetStyleAttribute(node, "display");
    auto visibility = GetStyleAttribute(node, "visibility");

    return (
        (display.empty() || display != "none") &&
        (visibility.empty() || visibility != "hidden") &&
        !HasAttribute(node, "hidden") &&
        (
            !HasAttribute(node, "aria-hidden") ||
            GetAttribute(node, "aria-hidden") != "true" ||
            GetNodeClass(node).find("fallback-image") != std::string::npos
        )
    );
    // old function definition
    /*
    if (node == nullptr || !IsElementNode(node))
        return false;

    if (!display.empty() && display == "none")
        return false;

    if (!visibility.empty() && visibility == "hidden")
        return false;

    if (HasAttribute(node, "hidden"))
        return false;

    if (HasAttribute(node, "aria-hidden") &&
        GetAttribute(node, "aria-hidden") == "true" &&
        GetNodeClass(node).find("fallback-image") == std::string::npos)
        return false;

    return true;
    */
}

// Initialize a node with base content score from its tag type
void ArticleScraper::InitializeNode(lxb_dom_node_t *node) {

  int readability = 0;

  auto tagName = GetTagName(node);
  if (tagName == "DIV") {
    readability += 5;
  } else if (tagName == "PRE" || tagName == "TD" || tagName == "BLOCKQUOTE") {
    readability += 3;
  } else if (tagName == "ADDRESS" || tagName == "OL" || tagName == "UL" ||
             tagName == "DL" || tagName == "DD" || tagName == "DT" ||
             tagName == "LI" || tagName == "FORM") {
    readability -= 3;
  } else if (tagName == "H1" || tagName == "H2" || tagName == "H3" ||
             tagName == "H4" || tagName == "H5" || tagName == "H6" ||
             tagName == "TH") {
    readability -= 5;
  }

  readability += GetClassWeight(node);
  SetAttribute(node, "readability", std::to_string(readability));
}

std::string ArticleScraper::GetInnerHtml(lxb_dom_node_t *node) {
  if (node == nullptr) {
    return std::string();
  }

  lexbor_str_t str = {0};
  lxb_status_t status;

  for (lxb_dom_node_t *child = GetFirstChild(node); child != nullptr;
       child = GetNextSibling(child)) {
    status = lxb_html_serialize_tree_str(child, &str);
    if (status != LXB_STATUS_OK) {
      break;
    }
  }

  std::string result;
  if (str.data != nullptr) {
    result.assign(reinterpret_cast<const char *>(str.data), str.length);

    lxb_dom_document_t *doc = node->owner_document;
  }

  return result;
}
