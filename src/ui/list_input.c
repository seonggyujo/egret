// Row hit testing and row clicks.
#include "ui/list_input.h"

#include "ui/layout.h"
#include "ui/row_actions.h"
#include "ui/rows_view.h"

bool list_buttons_on(const App *a, int vi) { return vi == a->hover || (a->hover < 0 && vi == a->selected); }

int list_hit(const App *a, int x, int y, int *vi) {
  *vi = layout_hit_row(a, x, y);
  const Listener *l = app_row(a, *vi);
  if (!l) return PART_NONE;
  RowParts p;
  layout_row_parts(a, layout_row(a, *vi), &p);
  if (row_is_confirming(a, l)) {
    if (box_contains(p.cancel, x, y)) return PART_CANCEL;
    if (box_contains(p.confirm, x, y)) return PART_CONFIRM;
    return PART_ROW;
  }
  if (!row_is_stopping(a, l)) {
    if (row_can_browse(l) && box_contains(p.browser, x, y)) return PART_BROWSER;
    if (row_has_folder(l) && box_contains(p.folder_button, x, y)) return PART_FOLDER;
    if (row_can_stop(l) && box_contains(p.stop, x, y)) return PART_STOP;
  }
  if (box_contains(p.folder, x, y)) return PART_FOLDER_TEXT;
  if (box_contains(p.tags, x, y)) return PART_TAGS;
  return PART_ROW;
}

void list_click(App *a, int part, int vi) {
  switch (part) {
    case PART_BROWSER: row_open_browser(a, vi); break;
    case PART_FOLDER: row_open_folder(a, vi); break;
    case PART_STOP: row_ask_stop(a, vi); break;
    case PART_CANCEL: row_cancel_stop(a); break;
    case PART_CONFIRM: row_confirm_stop(a); break;
    default:
      if (a->confirm.active && !row_is_confirming(a, app_row(a, vi))) a->confirm.active = false;
      rows_view_select(a, vi);
      break;
  }
}

void list_scroll(App *a, int rows) {
  int max_top = a->visible_count - layout_rows_shown(a);
  int top = a->top + rows;
  if (top > max_top) top = max_top;
  if (top < 0) top = 0;
  if (top == a->top) return;
  a->top = top;
  InvalidateRect(a->hwnd, NULL, FALSE);
}

bool list_mouse_on_confirm(const App *a) {
  POINT pt;
  GetCursorPos(&pt);
  ScreenToClient(a->hwnd, &pt);
  const Listener *l = app_row(a, layout_hit_row(a, pt.x, pt.y));
  return l && row_is_confirming(a, l);
}
