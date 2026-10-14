// Hit testing over the whole popup and handing clicks to their view.
#include "ui/mouse.h"

#include "ui/layout.h"
#include "ui/list_input.h"
#include "ui/popup.h"
#include "ui/row_actions.h"
#include "ui/rows_view.h"
#include "ui/settings_input.h"
#include "ui/settings_layout.h"
#include "ui/status.h"
#include "ui/tooltip.h"

static bool g_tracking;

// The part under (x, y); *vi gets the row's visible index when on a row.
static int hit(App *a, int x, int y, int *vi) {
  *vi = -1;
  if (a->view == POPUP_LIST && box_contains(layout_gear(a), x, y)) return PART_GEAR;
  if (a->view == POPUP_SETTINGS && box_contains(layout_back(a), x, y)) return PART_BACK;
  if (box_contains(status_link_box(a), x, y)) return PART_STATUS_LINK;
  if (a->view == POPUP_SETTINGS) return settings_hit(a, x, y);
  return list_hit(a, x, y, vi);
}

void mouse_move(App *a, int x, int y) {
  if (!g_tracking) {
    TRACKMOUSEEVENT t = {sizeof t, TME_LEAVE, a->hwnd, 0};
    g_tracking = TrackMouseEvent(&t);
  }
  int vi = -1, part = hit(a, x, y, &vi);
  if (part != a->hover_part || vi != a->hover) {
    a->hover_part = part;
    a->hover = vi;
    InvalidateRect(a->hwnd, NULL, FALSE);
  }
  tooltip_mouse_move(a, x, y);
}

void mouse_leave(App *a) {
  g_tracking = false;
  a->hover = -1;
  a->hover_part = PART_NONE;
  InvalidateRect(a->hwnd, NULL, FALSE);
}

void mouse_click(App *a, int x, int y) {
  int vi = -1, part = hit(a, x, y, &vi);
  if (part == PART_GEAR) popup_set_view(a, POPUP_SETTINGS);
  else if (part == PART_BACK) popup_set_view(a, POPUP_LIST);
  else if (part == PART_STATUS_LINK) status_link_click(a);
  else if (part >= PART_SETTINGS) settings_click(a, part);
  else if (vi >= 0) list_click(a, part, vi);
}

void mouse_double_click(App *a, int x, int y) {
  int vi = -1, part = hit(a, x, y, &vi);
  if (vi >= 0 && (part == PART_ROW || part == PART_FOLDER_TEXT || part == PART_TAGS)) row_open_browser(a, vi);
  else mouse_click(a, x, y);
}

void mouse_right_click(App *a, int x, int y) {
  int vi = layout_hit_row(a, x, y);
  if (vi < 0) return;
  rows_view_select(a, vi);
  POINT pt = {x, y};
  ClientToScreen(a->hwnd, &pt);
  row_menu(a, vi, pt);
}

void mouse_wheel(App *a, int delta) {
  if (a->view == POPUP_LIST) list_scroll(a, -delta / WHEEL_DELTA * 3);
}
