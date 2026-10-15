#ifndef CONTENT_BLOCK_RENDERER_HPP
#define CONTENT_BLOCK_RENDERER_HPP

#include <array>
#include <cctype>
#include <string_view>
#include <utility>

#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/flexbox_config.hpp>

#include "model/ContentBlock.hpp"

class ContentBlockRenderer {
public:
  static ftxui::Element Render(const ContentBlock& block) {
    return WalkBlock(block);
  }

  static ftxui::Element RenderInline(const ContentBlock& block) {
    WalkState state;
    WalkInLine(block, state);
    return state.Finish();
  }

private:
  struct WalkState
  {
    ftxui::Elements inline_;
    ftxui::Elements blocks_;

    void Flush()
    {
      if(inline_.empty())
        return;
      blocks_.push_back(ftxui::flexbox(
        std::move(inline_),
        ftxui::FlexboxConfig().SetGap(1, 0)));
      inline_.clear();
    }

    ftxui::Element Finish()
    {
      Flush();
      return ftxui::vbox(std::move(blocks_));
    }
  };

  static bool IsInNewLine(ContentBlockType tag)
  {
    return (tag == ContentBlockType::P ||
            tag == ContentBlockType::TABLE || tag == ContentBlockType::H1 ||
            tag == ContentBlockType::H2 || tag == ContentBlockType::H3 ||
            tag == ContentBlockType::BR || tag == ContentBlockType::HR ||
            tag == ContentBlockType::UL ||
            tag == ContentBlockType::DIV || tag == ContentBlockType::PRE ||
            tag == ContentBlockType::BLOCKQUOTE ||
            tag == ContentBlockType::CAPTION || tag == ContentBlockType::TR);
  }

  static ftxui::Element WalkBlock(const ContentBlock& block)
  {
    WalkState state;
    ftxui::Decorator dec = ftxui::nothing;
    switch (block.type)
    {
      case ContentBlockType::H1:
        dec = dec | ftxui::bold | ftxui::underlined;
        WalkChildren(block,state,dec);
        return state.Finish();
      case ContentBlockType::H2:
        dec = dec | ftxui::bold;
        WalkChildren(block,state,dec);
        return state.Finish();
      case ContentBlockType::H3:
        WalkChildren(block,state);
        return state.Finish();
      case ContentBlockType::P:
        WalkChildren(block,state);
        return ftxui::vbox({
          ftxui::separatorEmpty(),
          state.Finish()
        });
      case ContentBlockType::DIV:
        WalkChildren(block,state);
        return state.Finish();
      case ContentBlockType::BR:
        return ftxui::separatorEmpty();
      case ContentBlockType::HR:
        return ftxui::separatorLight();
      case ContentBlockType::BLOCKQUOTE:
        WalkChildren(block,state);
        return state.Finish() | ftxui::center | ftxui::borderLight;
      case ContentBlockType::PRE:
        WalkChildren(block,state);
        return state.Finish();
      case ContentBlockType::CAPTION:
        WalkChildren(block,state);
        return state.Finish() | ftxui::center;
      case ContentBlockType::TABLE:
        return VisitTABLE(block);
      case ContentBlockType::UL:
        return VisitUL(block,0);
      default:
        WalkChildren(block,state);
        return state.Finish();
    }
  }

  static ftxui::Element VisitTABLE(const ContentBlock& block)
  {
    ftxui::Elements rows;
    for(const auto& child : block.children)
    {
      if(child.type == ContentBlockType::CAPTION)
      {
        WalkState state;
        WalkChildren(child,state);
        rows.push_back(state.Finish() | ftxui::center);
      }
      else if(child.type == ContentBlockType::TR)
      {
        rows.push_back(VisitTR(child));
      }
    }

    return ftxui::vbox(rows) | ftxui::border | ftxui::center;
  }

  static ftxui::Element VisitTR(const ContentBlock& block)
  {
    ftxui::Elements col;
    for(const auto& child : block.children)
    {
      col.push_back(WalkBlock(child) | ftxui::border);
    }
    return ftxui::hbox(col) | ftxui::border ;
  }

  static ftxui::Element VisitUL(const ContentBlock& block, int depth)
  {
    ftxui::Elements rows;
    for (const auto& child : block.children) rows.push_back(VisitLI(child, depth));
    return ftxui::vbox(std::move(rows));
  }

  static ftxui::Element VisitLI(const ContentBlock& block, int depth)
  {
    WalkState state;
    state.inline_.push_back(
      ftxui::text(listDots_[depth % listDots_.size()]));
    for (const auto& child : block.children) {
      if (child.type == ContentBlockType::UL) {
        state.Flush();
        state.blocks_.push_back(VisitUL(child, depth + 1));
      } else if (IsInNewLine(child.type)) {
        state.Flush();
        state.blocks_.push_back(WalkBlock(child));
      } else {
        WalkInLine(child,state);
      }
    }
    return  state.Finish();
  }

  static void WalkInLineChildren(
    const ContentBlock& block,
    WalkState& state,
    ftxui::Decorator dec = ftxui::nothing
  )
  {
    for (const auto& child : block.children) WalkInLine(child, state, dec);
  }

  static void WalkInLine(
    const ContentBlock& block,
    WalkState& state,
    ftxui::Decorator dec = ftxui::nothing
  )
  {
    switch (block.type) {
      case ContentBlockType::TEXT:
        PushText(block.text,state,dec);
        break;
      case ContentBlockType::A:
        WalkInLineChildren(block, state, dec | ftxui::hyperlink(block.url));
        break;
      case ContentBlockType::I:
        WalkInLineChildren(block, state, dec | ftxui::italic);
        break;
      case ContentBlockType::U:
        WalkInLineChildren(block, state, dec | ftxui::underlined);
        break;
      case ContentBlockType::SPAN:
        WalkInLineChildren(block, state, dec);
        break;
      case ContentBlockType::B:
        WalkInLineChildren(block, state, dec | ftxui::bold);
        break;
      case ContentBlockType::CODE:
        WalkInLineChildren(block, state, dec);
        break;
      case ContentBlockType::IMG:
         state.inline_.push_back(ftxui::text("📸") | ftxui::hyperlink(block.url));
        break;
      default:
        state.Flush();
        state.blocks_.push_back(WalkBlock(block) | dec);
        break;
    }
  };

  static void PushText(
    std::string_view text,
    WalkState& state,
    ftxui::Decorator dec = ftxui::nothing
  )
  {
    size_t i = 0, n = text.size();
     while (i < n) {
       while (i < n && std::isspace((unsigned char)text[i])) ++i;
       size_t start = i;
       while (i < n && !std::isspace((unsigned char)text[i])) ++i;
       if (start != i) PushWord(text.substr(start, i - start),state,dec);
     }
  }

  static void PushWord(
    std::string_view word,
    WalkState& state,
    ftxui::Decorator dec = ftxui::nothing
  )
  {
    state.inline_.push_back(ftxui::text(word) | dec);
  }

  static void WalkChildren(const ContentBlock& block, WalkState& state, ftxui::Decorator dec = ftxui::nothing)
  {
    for(const auto& child  : block.children)
    {
      if(IsInNewLine(child.type))
      {
        state.Flush();
        state.blocks_.push_back(WalkBlock(child) | dec);
      }
      else
      {
        WalkInLine(child,state,dec);
      }

    }
    state.Flush();
  }

  static constexpr std::array<std::string_view, 3> listDots_ = {"•", "◦", "▪"};
};

#endif // CONTENT_BLOCK_RENDERER_HPP
