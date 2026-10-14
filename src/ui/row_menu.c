// The right-click menu on a row.
#include <stdio.h>

#include "app/strings.h"
#include "ui/clipboard.h"
#include "ui/row_actions.h"
#include "ui/rows_view.h"

enum { MENU_BROWSER = 1, MENU_FOLDER, MENU_COPY, MENU_STOP, MENU_HIDE };

static void append(HMENU menu, UINT id, const char *text, bool enabled) {
  wchar_t w[256];
  MultiByteToWideChar(CP_UTF8, 0, text, -1, w, 256);
  AppendMenuW(menu, MF_STRING | (enabled ? 0 : MF_GRAYED), id, w);
}

static void hide_program(App *a, const char *exe) {
  hidden_add(&a->hidden, exe);
  rows_view_refilter(a);
  rows_view_select(a, a->selected);
  rows_view_update_tray(a);
}

void row_menu(App *a, int vi, POINT pt) {
  const Listener *l = app_row(a, vi);
  if (!l) return;
  const Strings *s = strings();
  char hide[200];
  format_hide_program(hide, sizeof hide, l->exe[0] ? l->exe : l->name);
  HMENU menu = CreatePopupMenu();
  append(menu, MENU_BROWSER, s->menu_browser, row_can_browse(l));
  append(menu, MENU_FOLDER, s->menu_folder, row_has_folder(l));
  append(menu, MENU_COPY, s->menu_copy_port, true);
  AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
  append(menu, MENU_STOP, s->menu_stop, row_can_stop(l));
  append(menu, MENU_HIDE, hide, l->exe[0] && !hidden_contains(&a->hidden, l->exe));
  int cmd = TrackPopupMenuEx(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, a->hwnd, NULL);
  DestroyMenu(menu);
  char port[8];
  snprintf(port, sizeof port, "%u", l->port);
  switch (cmd) {
    case MENU_BROWSER: row_open_browser(a, vi); break;
    case MENU_FOLDER: row_open_folder(a, vi); break;
    case MENU_COPY: clipboard_set(a->hwnd, port); break;
    case MENU_STOP: row_ask_stop(a, vi); break;
    case MENU_HIDE: hide_program(a, l->exe); break;
    default: break;
  }
}
