// One row: port, program, folder, tags and uptime, or its buttons, or the
// yellow confirm, or "stopping".
#include <stdio.h>

#include "actions/stop.h"
#include "app/strings.h"
#include "app/theme.h"
#include "base/clock.h"
#include "model/text.h"
#include "ui/layout.h"
#include "ui/list_input.h"
#include "ui/list_paint.h"
#include "ui/row_actions.h"
#include "ui/row_texts.h"
#include "ui/widgets.h"

static void text_in(App *a, UiFont *f, const char *s, Box b, uint32_t color, TextAlign align) {
  ui_text_draw(a->canvas, f, s, (float)b.x, (float)b.y, (float)b.w, (float)b.h, color, align);
}

static void paint_confirm(App *a, const Listener *l, const RowParts *p) {
  const ThemeColors *t = theme();
  char text[300];
  if (a->confirm.admin) format_confirm_admin(text, sizeof text, l->name, l->service);
  else format_confirm(text, sizeof text, l->name, l->pid);
  text_in(a, a->font, text, p->text, t->solid, ALIGN_LEFT);
  bool hover_cancel = a->hover_part == PART_CANCEL, hover_ok = a->hover_part == PART_CONFIRM;
  widget_text_button(a, p->cancel, strings()->cancel, "Esc", hover_cancel ? t->strike_line : 0, t->strike_text2,
                     t->solid, t->strike_text2);
  widget_text_button(a, p->confirm, strings()->stop, "Enter", hover_ok ? 0xFF3A3A3A : t->solid, t->solid,
                     t->solid_text, 0xFFB8B8B8);
}

static void paint_buttons(App *a, const Listener *l, const RowParts *p) {
  if (row_can_browse(l)) widget_icon_button(a, p->browser, ICON_BROWSER, a->hover_part == PART_BROWSER);
  if (row_has_folder(l)) widget_icon_button(a, p->folder_button, ICON_FOLDER, a->hover_part == PART_FOLDER);
  if (row_can_stop(l)) {
    IconId icon = stop_needs_admin(l) ? ICON_LOCK : ICON_POWER;
    widget_icon_button(a, p->stop, icon, a->hover_part == PART_STOP);
  }
}

static void paint_tags(App *a, const Listener *l, Box b) {
  RowTag tags[2];
  int n = row_tags(l, tags);
  float x = (float)b.x, gap = 6 * a->scale;
  canvas_push_clip(a->canvas, (float)b.x, (float)b.y, (float)b.w, (float)b.h);
  for (int i = 0; i < n; i++) {
    float w = ui_text_width(a->font_tag, tags[i].text);
    ui_text_draw(a->canvas, a->font_tag, tags[i].text, x, (float)b.y, w + 1, (float)b.h, tags[i].color, ALIGN_LEFT);
    x += w + gap;
  }
  canvas_pop_clip(a->canvas);
}

static uint32_t row_fill(const App *a, int vi, const Listener *l) {
  const ThemeColors *t = theme();
  if (row_is_confirming(a, l)) return t->strike;
  if (vi == a->selected) return t->selected;
  if (vi == a->hover) return t->hover;
  return 0;
}

void row_paint(App *a, int vi, Box row) {
  const Listener *l = app_row(a, vi);
  if (!l) return;
  const ThemeColors *t = theme();
  uint32_t fill = row_fill(a, vi, l);
  if (fill) canvas_fill_rect(a->canvas, (float)row.x, (float)row.y, (float)row.w, (float)row.h, fill);
  else canvas_fill_rect(a->canvas, (float)row.x, (float)(row.y + row.h - 1), (float)row.w, 1, t->line);
  RowParts p;
  layout_row_parts(a, row, &p);
  bool system = l->kind == KIND_SYSTEM, confirming = row_is_confirming(a, l);
  char port[8];
  snprintf(port, sizeof port, "%u", l->port);
  uint32_t ink = confirming ? t->solid : system ? t->dim : t->text;
  text_in(a, a->font_port, port, p.port, ink, ALIGN_LEFT);
  if (confirming) {
    paint_confirm(a, l, &p);
    return;
  }
  text_in(a, a->font, l->name, p.name, system ? t->text2 : t->text, ALIGN_LEFT);
  char folder[RAW_PATH];
  row_folder_text(l, folder, sizeof folder);
  if (row_is_stopping(a, l)) {
    text_in(a, a->font, strings()->stopping, p.folder, t->text2, ALIGN_LEFT);
    return;
  }
  bool buttons = list_buttons_on(a, vi);
  int folder_right = buttons ? p.browser.x - app_px(a, 6) : p.folder.x + p.folder.w;
  Box folder_box = {p.folder.x, p.folder.y, folder_right - p.folder.x, p.folder.h};
  text_in(a, a->font, folder, folder_box, system ? t->dim : t->text2, ALIGN_LEFT);
  if (buttons) {
    paint_buttons(a, l, &p);
    return;
  }
  paint_tags(a, l, p.tags);
  char up[32];
  text_uptime(up, sizeof up, l->started, clock_filetime_now());
  text_in(a, a->font_num, up, p.up, system ? t->dim : t->text2, ALIGN_RIGHT);
}
