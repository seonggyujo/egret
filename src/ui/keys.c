// What each key does in the list and in the settings.
#include "ui/keys.h"

#include "ui/layout.h"
#include "ui/popup.h"
#include "ui/row_actions.h"
#include "ui/rows_view.h"
#include "ui/settings_input.h"

static void move_selection(App *a, int by) {
  a->confirm.active = false;
  rows_view_select(a, a->selected < 0 ? 0 : a->selected + by);
}

static void escape(App *a) {
  if (a->confirm.active) row_cancel_stop(a);
  else if (a->query[0]) SetWindowTextW(a->search, L"");  // EN_CHANGE refilters
  else popup_hide(a);
}

bool keys_list(App *a, UINT vk) {
  switch (vk) {
    case VK_DOWN: move_selection(a, 1); return true;
    case VK_UP: move_selection(a, -1); return true;
    case VK_NEXT: move_selection(a, MAX_ROWS); return true;
    case VK_PRIOR: move_selection(a, -MAX_ROWS); return true;
    case VK_RETURN:
      if (a->confirm.active) row_confirm_stop(a);
      else row_open_browser(a, a->selected);
      return true;
    case VK_DELETE: row_ask_stop(a, a->selected); return true;
    case VK_ESCAPE: escape(a); return true;
    default: return false;
  }
}

bool keys_popup(App *a, UINT vk) {
  if (a->view == POPUP_LIST) return keys_list(a, vk);
  if (settings_capture_key(a, vk)) return true;
  if (vk != VK_ESCAPE) return false;
  popup_set_view(a, POPUP_LIST);
  return true;
}
