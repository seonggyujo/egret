// Filtering the rows into the visible list and keeping the selection.
#include "ui/rows_view.h"

#include <stdlib.h>

#include "app/prefs.h"
#include "base/mem.h"
#include "model/filter.h"
#include "ui/layout.h"
#include "ui/popup.h"
#include "ui/tray.h"

static int hidden_names(const App *a, const char **names) {
  return hidden_pointers(&a->hidden, names, HIDDEN_MAX);
}

void rows_view_refilter(App *a) {
  const char *names[HIDDEN_MAX];
  RowFilter f = {a->query, a->show_all, names, hidden_names(a, names)};
  if (a->visible_cap < a->rows.count) {
    a->visible_cap = a->rows.count;
    a->visible = xrealloc(a->visible, sizeof *a->visible * (size_t)a->visible_cap);
  }
  a->visible_count = 0;
  for (int i = 0; i < a->rows.count; i++) {
    if (row_visible(&a->rows.items[i], &f)) a->visible[a->visible_count++] = i;
  }
}

int rows_view_find(const App *a, uint16_t port, uint32_t pid) {
  for (int vi = 0; vi < a->visible_count; vi++) {
    const Listener *l = app_row(a, vi);
    if (l->port == port && l->pid == pid) return vi;
  }
  return -1;
}

void rows_view_select(App *a, int vi) {
  if (a->visible_count == 0) {
    a->selected = -1;
    return;
  }
  if (vi < 0) vi = 0;
  if (vi >= a->visible_count) vi = a->visible_count - 1;
  a->selected = vi;
  int shown = layout_rows_shown(a);
  if (vi < a->top) a->top = vi;
  if (vi >= a->top + shown) a->top = vi - shown + 1;
  InvalidateRect(a->hwnd, NULL, FALSE);
}

void rows_view_update_tray(App *a) {
  const char *names[HIDDEN_MAX];
  int shown = 0, tucked = 0;
  rows_count(&a->rows, names, hidden_names(a, names), &shown, &tucked);
  tray_set_count(a, shown);
}

void rows_view_take(App *a, ListenerList *fresh) {
  const Listener *was = app_row(a, a->selected);
  uint16_t port = was ? was->port : 0;
  uint32_t pid = was ? was->pid : 0;
  int old_count = a->visible_count;
  listener_list_free(&a->rows);
  a->rows = *fresh;
  free(fresh);
  rows_view_refilter(a);
  int vi = was ? rows_view_find(a, port, pid) : -1;
  if (vi < 0 && a->visible_count) vi = a->selected < 0 ? 0 : a->selected;
  if (a->top > 0 && a->top + layout_rows_shown(a) > a->visible_count) {
    a->top = a->visible_count > MAX_ROWS ? a->visible_count - MAX_ROWS : 0;
  }
  rows_view_select(a, vi);
  if (a->confirm.active && rows_view_find(a, a->confirm.port, a->confirm.pid) < 0) a->confirm.active = false;
  if (a->shown && (old_count != a->visible_count)) popup_fit(a);
  if (a->shown) InvalidateRect(a->hwnd, NULL, FALSE);
  rows_view_update_tray(a);
}

void rows_view_toggle_all(App *a) {
  a->show_all = !a->show_all;
  prefs_set_bool(PREF_SHOW_ALL, a->show_all);
  rows_view_refilter(a);
  rows_view_select(a, a->selected);
  if (a->shown) popup_fit(a);
}
