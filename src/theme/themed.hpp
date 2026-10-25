#ifndef THEMED_HPP
#define THEMED_HPP

#include "theme/themes.hpp"
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <string_view>
namespace themed {

   enum class Tone { Accent, Muted, Normal};
   enum class ParagraphAlignment { Center, Justify, Left, Right, Normal };

  inline Pallet Palette()
  {
    return Theme::GetInstance().GetPallet();
  }


  inline ftxui::Decorator Accent()
  {
    return ftxui::color(Palette().primary);
  }

  inline ftxui::Decorator Muted()
  {
    return ftxui::color(Palette().secondary);
  }

  inline ftxui::Decorator Base()
  {
    auto th = Palette();
    return ftxui::bgcolor(th.bg) | ftxui::color(th.fg);
  }

  inline ftxui::Decorator Selected()
  {
    auto th = Palette();
    return ftxui::bgcolor(th.focus_bg) | ftxui::color(th.focus_fg);
  }

  inline ftxui::Element Text(std::string text, Tone tone = Tone::Normal)
  {
    auto th = Palette();
    auto txt = ftxui::text(text);
    switch (tone) {
      case Tone::Accent:
        txt = txt | ftxui::color(th.primary);
        break;
      case Tone::Muted:
        txt = txt | ftxui::color(th.secondary);
        break;
      case Tone::Normal:
        break;
    }

    return txt;
  }

  inline ftxui::Element Paragraph(std::string text, Tone tone = Tone::Normal, ParagraphAlignment alignment = ParagraphAlignment::Normal )
  {
    auto th = Palette();
    ftxui::Element txt;

    switch (alignment) {
      case ParagraphAlignment::Center:
        txt = ftxui::paragraphAlignCenter(text);
        break;
      case ParagraphAlignment::Normal:
        txt = ftxui::paragraph(text);
        break;
      case ParagraphAlignment::Justify:
        txt = ftxui::paragraphAlignJustify(text);
        break;
      case ParagraphAlignment::Left:
        txt = ftxui::paragraphAlignLeft(text);
        break;
      case ParagraphAlignment::Right:
        txt = ftxui::paragraphAlignRight(text);
        break;
    }


    switch (tone) {
      case Tone::Accent:
        txt = txt | ftxui::color(th.primary);
        break;
      case Tone::Muted:
        txt = txt | ftxui::color(th.secondary);
        break;
      case Tone::Normal:
        txt = txt | ftxui::color(th.fg);
        break;
    }

    return txt;
  }



  inline ftxui::Element Separator()
  {
    return ftxui::separator() | ftxui::color(Palette().border);
  }

  inline ftxui::Element SeparatorCharacter(std::string_view sequence)
  {
    return ftxui::separatorCharacter(sequence) | ftxui::color(Palette().border);
  }


  inline ftxui::Decorator Border(ftxui::BorderStyle style = ftxui::LIGHT) {
    return ftxui::borderStyled(style, Palette().border);
  }

  inline ftxui::ButtonOption ButtonOptions() {
    auto opt = ftxui::ButtonOption::Simple();
    opt.transform = [](const ftxui::EntryState& state) {
      auto content = ftxui::text(state.label);
      if (state.focused)
        content = content | Selected();
      return content | Border();
    };
    return opt;
  }

  inline ftxui::MenuOption MenuOptions(std::vector<std::string>& entries,
                                       int* selected) {
    auto opt = ftxui::MenuOption::Vertical();
    opt.entries = &entries;
    opt.selected = selected;
    opt.entries_option.transform = [](const ftxui::EntryState& state) {
      auto e = ftxui::text(state.label);
      if (state.focused)
        e |= Selected();
      if (state.active)
        e |= ftxui::bold;
      return e;
    };
    return opt;
  }



}





#endif
