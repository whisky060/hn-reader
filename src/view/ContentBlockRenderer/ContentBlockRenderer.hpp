#ifndef CONTENT_BLOCK_RENDERER_HPP
#define CONTENT_BLOCK_RENDERER_HPP

#include <array>
#include <cctype>
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/deprecated.hpp>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <image_view.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include "model/ContentBlock/ContentBlock.hpp"
#include "theme/themed.hpp"
#include "widgets/Scroll/CullNode.hpp"

class ContentBlockRenderer {
public:
  static ftxui::Element Render(const ContentBlock& block) {
    return WalkBlock(block);
  }

  static ftxui::Component ContentComponent(const ContentBlock& block)
  {
    return ftxui::Renderer(CachedRender(std::make_shared<ContentBlock>(block)));
  }

  static std::function<ftxui::Element()> CachedRender(std::shared_ptr<const ContentBlock> block)
  {
    struct Cache
    {
      std::shared_ptr<const ContentBlock> block;
      ftxui::Element elem;
      std::string theme;
    };

    auto cache = std::make_shared<Cache>();
    cache->block = std::move(block);

    return [cache]() -> ftxui::Element
    {
      auto theme = Theme::GetInstance().GetActiveTheme();
      if(theme != cache->theme)
      {
        cache->elem = WalkBlock(*cache->block);
        cache->theme = theme;
      }
      return cache->elem;
    };
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
      blocks_.push_back(ftxui::flexbox(std::move(inline_)));
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
            tag == ContentBlockType::CAPTION || tag == ContentBlockType::TR ||
            tag == ContentBlockType::IMG);
  }

  static ftxui::Element WalkBlock(const ContentBlock& block)
  {
    WalkState state;
    ftxui::Decorator dec = ftxui::nothing;
    switch (block.type)
    {
      case ContentBlockType::H1:
        dec = dec | ftxui::bold | ftxui::underlined | themed::Accent();
        WalkChildren(block,state,dec);
        return ftxui::vbox({
          ftxui::separatorEmpty(),
          state.Finish(),
          ftxui::separatorEmpty()
        });
      case ContentBlockType::H2:
        dec = dec | ftxui::bold | themed::Accent();
        WalkChildren(block,state,dec);
        return ftxui::vbox({
          ftxui::separatorEmpty(),
          state.Finish(),
          ftxui::separatorEmpty()
        });
      case ContentBlockType::H3:
        dec = themed::Accent();
        WalkChildren(block,state, dec);
        return ftxui::vbox({
          ftxui::separatorEmpty(),
          state.Finish(),
          ftxui::separatorEmpty()
        });
      case ContentBlockType::P:
        WalkChildren(block,state);
        return ftxui::vbox({
          state.Finish()
        });
      case ContentBlockType::DIV:
        WalkChildren(block,state);
        return state.Finish();
      case ContentBlockType::BR:
        return ftxui::separatorEmpty();
      case ContentBlockType::HR:
        return themed::Separator();
      case ContentBlockType::BLOCKQUOTE:
        WalkChildren(block,state);
        return state.Finish() | ftxui::center | themed::Border(ftxui::BorderStyle::LIGHT);
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
      case ContentBlockType::IMG:
        return VisitIMG(block);
      default:
        WalkChildren(block,state);
        return state.Finish();
    }
  }

  static ftxui::Element VisitTABLE(const ContentBlock& block)
  {

    std::vector<ftxui::Elements> rows;
    ftxui::Element caption = nullptr;
    for(const auto& child : block.children)
    {
      if(child.type == ContentBlockType::CAPTION)
      {
        WalkState state;
        WalkChildren(child,state);
        caption = state.Finish() | ftxui::center;
      }
      else if(child.type == ContentBlockType::TR)
      {
        rows.push_back(VisitTR(child));
      }
      else if(child.type == ContentBlockType::TBODY)
      {
        auto row = VisitTBody(child);
        rows.insert(rows.end(),row.begin(),row.end());
      }
      else if(child.type == ContentBlockType::THEAD)
      {
        auto row = VisitTHead(child);
        rows.insert(rows.end(),row.begin(),row.end());
      }
      else if(child.type == ContentBlockType::TFOOT)
      {
        auto row = VisitTFoot(child);
        rows.insert(rows.end(),row.begin(),row.end());
      }
    }
    auto border = ftxui::color(themed::Palette().border);
    ftxui::Table table(std::move(rows));
    auto content = table.SelectAll();
    content.Separator(ftxui::BorderStyle::LIGHT,border);
    content.Border(ftxui::BorderStyle::LIGHT,border);

    if(caption != nullptr)
      return ftxui::vbox({
        caption,
        table.Render() | ftxui::center
      });

    return table.Render() | ftxui::center;
  }

  static std::vector<ftxui::Elements> VisitTBody(const ContentBlock& block)
  {
    std::vector<ftxui::Elements> rows;
    for(auto& child : block.children)
    {
      if(child.type == ContentBlockType::TR)
      {
       rows.push_back(VisitTR(child));
      }
    }
    return rows;
  }

  static std::vector<ftxui::Elements> VisitTHead(const ContentBlock& block)
  {
    std::vector<ftxui::Elements> rows;
    for(auto& child : block.children)
    {
      if(child.type == ContentBlockType::TR)
      {
       rows.push_back(VisitTR(child) | ftxui::bold | themed::Accent());
      }
    }
    return rows;
  }
  static std::vector<ftxui::Elements> VisitTFoot(const ContentBlock& block)
  {
    std::vector<ftxui::Elements> rows;
    for(auto& child : block.children)
    {
      if(child.type == ContentBlockType::TR)
      {
       rows.push_back(VisitTR(child) | ftxui::bold | themed::Accent());
      }
    }
    return rows;
  }

  static ftxui::Elements VisitTR(const ContentBlock& block)
  {
    ftxui::Elements col;
    for(const auto& child : block.children)
    {
      if(child.type == ContentBlockType::TD)
        col.push_back(WalkBlock(child) | ftxui::center);
    }
    return col;
  }

  static ftxui::Element VisitUL(const ContentBlock& block, int depth)
  {
    ftxui::Elements rows;
    for (const auto& child : block.children)
      if(child.type == ContentBlockType::LI)
        rows.push_back( ftxui::hbox({
          ftxui::text(std::string(listDots_[depth % listDots_.size()]) + " "),
          VisitLI(child, depth)
        }));
    return ftxui::vbox(std::move(rows));
  }

  static ftxui::Element VisitLI(const ContentBlock& block, int depth)
  {

    WalkState state;
    for (const auto& child : block.children) {
      if (child.type == ContentBlockType::UL) {
        state.Flush();
        state.blocks_.push_back(ftxui::hbox({
          ftxui::separatorEmpty(),
          VisitUL(child, depth + 1)
        }));
      } else if (IsInNewLine(child.type)) {
        state.Flush();
        state.blocks_.push_back(WalkBlock(child));
      } else {
        WalkInLine(child,state);
      }
    }
    return  state.Finish();
  }

  static ftxui::Element VisitIMG(const ContentBlock& block)
  {
    auto dimensions = ftxui::Terminal::Size();
    static const float imageScalingPrecentage{0.90f};
    return CullNode(ftxui::image_view(block.url)
      | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, static_cast<int>(dimensions.dimy * imageScalingPrecentage))
     // | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, static_cast<int>(dimensions.dimx * imageScalingPrecentage))
      | ftxui::center);
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
        WalkInLineChildren(block, state, dec | themed::Accent() | ftxui::hyperlink(block.url) );
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
        state.Flush();
        state.blocks_.push_back(VisitIMG(block));
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
    std::string s(text);
    for (char& c : s)
      if (std::isspace((unsigned char)c))
        c = ' ';

    size_t first = s.find_first_not_of(' ');
    if (first == std::string::npos)
    {
      if (!state.inline_.empty())
        state.inline_.push_back(ftxui::text(std::move(s)) | dec);
      return;
    }
    if (state.inline_.empty())
      s.erase(0, first);

    size_t i = 0, n = s.size();
    while (i < n)
    {
      size_t start = i;
      while (i < n && s[i] == ' ') ++i;
      while (i < n && s[i] != ' ') ++i;
      while (i < n && s[i] == ' ') ++i;
      state.inline_.push_back(ftxui::text(s.substr(start, i - start)) | dec);
    }
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
