#ifndef BASEVIEW_H
#define BASEVIEW_H

#include <ftxui/component/component_base.hpp>
#include <string>
#include <vector>

/*
 *  Metadata describing a single keyboard shortcut available in a view.
 *  Kept intentionally small: key + human readable label,
 *  so the template bottom bar can render it without knowing view logic.
 *  e.g. {"Esc", "back"}, {"Enter", "open"}, {"Ctrl-O", "browser"}
 */
struct Shortcut {
  std::string key;
  std::string label;
};

/*
 *  Base class for every full-screen view hosted by a template.
 *  The template receives a BaseView to render (via Push/ChangeView) and
 *  queries GetTitle()/GetShortcuts() every frame to update its chrome
 *  (header / bottom bar) without view-specific if/else chains.
 *
 *  Rule: every key handled in OnEvent() must be listed in GetShortcuts().
 */
class BaseView : public ftxui::ComponentBase {
 public:
  BaseView() = default;
  virtual ~BaseView() = default;

  virtual std::string GetTitle() const = 0;
  virtual std::vector<Shortcut> GetShortcuts() const = 0;
};

#endif
