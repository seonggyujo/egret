// The column titles, the rows, the scroll bar and the empty state.
#include "ui/list_paint.h"

#include "app/strings.h"
#include "app/theme.h"
#include "ui/layout.h"

static void paint_titles(App *a) {
  const ThemeColors *t = theme();
  const Strings *s = strings();
  Box h = layout_colhead(a);
  RowParts p;
  layout_row_parts(a, h, &p);
  float y = (float)h.y, hh = (float)h.h;
  ui_text_draw(a->canvas, a->font_small, s->col_port, (float)p.port.x, y, (float)p.port.w, hh, t->text2, ALIGN_LEFT);
  ui_text_draw(a->canvas, a->font_small, s->col_process, (float)p.name.x, y, (float)p.name.w, hh, t->text2,
               ALIGN_LEFT);
  ui_text_draw(a->canvas, a->font_small, s->col_folder, (float)p.folder.x, y, (float)p.folder.w, hh, t->text2,
               ALIGN_LEFT);
  ui_text_draw(a->canvas, a->font_small, s->col_uptime, (float)p.up.x, y, (float)p.up.w, hh, t->text2, ALIGN_RIGHT);
  canvas_fill_rect(a->canvas, 0, y + hh - 1, (float)h.w, 1, t->line);
}

static void paint_empty(App *a) {
  const ThemeColors *t = theme();
  const Strings *s = strings();
  Box b = layout_body(a);
  float x = (float)app_px(a, 18), w = b.w - 2 * x;
  if (a->query[0]) {
    char text[300];
    format_no_match(text, sizeof text, a->query);
    ui_text_draw(a->canvas, a->font, text, x, (float)b.y, w, (float)b.h, t->text2, ALIGN_LEFT);
    return;
  }
  float mid = b.y + b.h / 2.0f, line = (float)app_px(a, 20);
  ui_text_draw(a->canvas, a->font_title, s->empty_title, x, mid - line, w, line, t->text, ALIGN_LEFT);
  ui_text_draw(a->canvas, a->font, s->empty_hint, x, mid + app_px(a, 2), w, line, t->text2, ALIGN_LEFT);
}

static void paint_scroll_bar(App *a) {
  int shown = layout_rows_shown(a);
  if (a->visible_count <= shown) return;
  Box b = layout_body(a);
  float track = (float)b.h - 2 * app_px(a, 4);
  float h = track * shown / a->visible_count, y = b.y + app_px(a, 4) + track * a->top / a->visible_count;
  float w = 3 * a->scale;
  canvas_fill_round(a->canvas, b.x + b.w - w - app_px(a, 3), y, w, h, w / 2, theme()->scroll);
}

void list_paint(App *a) {
  if (a->visible_count == 0) {
    paint_empty(a);
    return;
  }
  paint_titles(a);
  for (int slot = 0; slot < layout_rows_shown(a); slot++) {
    int vi = a->top + slot;
    row_paint(a, vi, layout_row(a, vi));
  }
  paint_scroll_bar(a);
}
