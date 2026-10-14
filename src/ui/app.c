// Sizes and fonts at the current DPI, and small helpers on the app state.
#include "ui/app.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define NOTE_MS 5000

int app_px(const App *a, float css) { return (int)lroundf(css * a->scale); }

static void set_metrics(App *a) {
  Metrics *m = &a->m;
  m->width = app_px(a, 520);
  m->header_h = app_px(a, 48);
  m->colhead_h = app_px(a, 26);
  m->row_h = app_px(a, 34);
  m->status_h = app_px(a, 28);
  m->empty_h = app_px(a, 84);
  m->pad_x = app_px(a, 14);
  m->gap = app_px(a, 10);
  m->col_port = app_px(a, 56);
  m->col_name = app_px(a, 104);
  m->col_tags = app_px(a, 84);
  m->col_up = app_px(a, 52);
  m->button_w = app_px(a, 28);
  m->button_h = app_px(a, 26);
}

void app_free_fonts(App *a) {
  UiFont **fonts[] = {&a->font, &a->font_small, &a->font_tag, &a->font_title, &a->font_port, &a->font_num};
  for (size_t i = 0; i < sizeof fonts / sizeof fonts[0]; i++) {
    ui_font_free(*fonts[i]);
    *fonts[i] = NULL;
  }
  if (a->search_font) DeleteObject(a->search_font);
  a->search_font = NULL;
}

void app_set_dpi(App *a, UINT dpi) {
  a->dpi = dpi;
  a->scale = dpi / 96.0f;
  set_metrics(a);
  app_free_fonts(a);
  const wchar_t *ui = L"Segoe UI Variable Text", *num = L"Bahnschrift";
  a->font = ui_font_new(ui, 12 * a->scale, 400);
  a->font_small = ui_font_new(ui, 11 * a->scale, 400);
  a->font_tag = ui_font_new(ui, 11 * a->scale, 600);
  a->font_title = ui_font_new(ui, 13.5f * a->scale, 600);
  a->font_port = ui_font_new(num, 15 * a->scale, 600);
  a->font_num = ui_font_new(num, 12 * a->scale, 400);
  a->search_font = CreateFontW(-app_px(a, 14), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                               OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH,
                               L"Segoe UI");
}

const Listener *app_row(const App *a, int visible_index) {
  if (visible_index < 0 || visible_index >= a->visible_count) return NULL;
  return &a->rows.items[a->visible[visible_index]];
}

void app_note(App *a, const char *text, bool error) {
  snprintf(a->note, sizeof a->note, "%s", text);
  a->note_error = error;
  a->note_until = GetTickCount64() + NOTE_MS;
  InvalidateRect(a->hwnd, NULL, FALSE);
}
