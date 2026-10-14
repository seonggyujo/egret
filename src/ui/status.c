// The status bar's text, its link, and what the link does.
#include "ui/status.h"

#include <stdio.h>

#include "app/prefs.h"
#include "app/strings.h"
#include "app/theme.h"
#include "app/version.h"
#include "model/filter.h"
#include "ui/layout.h"
#include "ui/rows_view.h"
#include "ui/widgets.h"

// The left text and the link's text (NULL for none).
static const char *texts(App *a, char *left, size_t cap) {
  const Strings *s = strings();
  if (a->view == POPUP_SETTINGS) {
    snprintf(left, cap, "egret %s", EGRET_VERSION);
    return s->open_settings_folder;
  }
  if (a->note[0]) {
    snprintf(left, cap, "%s", a->note);
    return NULL;
  }
  if (a->show_all) {
    format_status_all(left, cap, a->rows.count);
    return s->hide;
  }
  const char *names[HIDDEN_MAX];
  int shown = 0, tucked = 0;
  rows_count(&a->rows, names, hidden_pointers(&a->hidden, names, HIDDEN_MAX), &shown, &tucked);
  format_status(left, cap, shown, tucked);
  return tucked ? s->show : NULL;
}

Box status_link_box(App *a) {
  char left[256];
  const char *link = texts(a, left, sizeof left);
  if (!link) return (Box){0};
  Box bar = layout_status(a);
  int w = (int)ui_text_width(a->font_small, link) + app_px(a, 8), pad = app_px(a, 14);
  if (a->view == POPUP_SETTINGS) return (Box){bar.x + bar.w - pad - w, bar.y, w, bar.h};
  int x = bar.x + pad + (int)ui_text_width(a->font_small, left) + app_px(a, 4);
  return (Box){x, bar.y, w, bar.h};
}

void status_paint(App *a) {
  const ThemeColors *t = theme();
  Box bar = layout_status(a);
  canvas_fill_rect(a->canvas, (float)bar.x, (float)bar.y, (float)bar.w, (float)bar.h, t->status_bg);
  canvas_fill_rect(a->canvas, (float)bar.x, (float)bar.y, (float)bar.w, 1, t->header_line);
  char left[256];
  const char *link = texts(a, left, sizeof left);
  uint32_t color = a->view == POPUP_LIST && a->note[0] && a->note_error ? t->error : t->text2;
  float pad = (float)app_px(a, 14);
  ui_text_draw(a->canvas, a->font_small, left, bar.x + pad, (float)bar.y, bar.w - 2 * pad, (float)bar.h, color,
               ALIGN_LEFT);
  if (!link) return;
  Box b = status_link_box(a);
  widget_link(a, b, link, a->hover_part == PART_STATUS_LINK, ALIGN_CENTER);
}

void status_link_click(App *a) {
  if (a->view == POPUP_LIST) {
    rows_view_toggle_all(a);
    return;
  }
  wchar_t folder[MAX_PATH];
  if (prefs_folder(folder, MAX_PATH)) ShellExecuteW(NULL, L"open", folder, NULL, NULL, SW_SHOWNORMAL);
}
