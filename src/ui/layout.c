// The list view's boxes from the metrics at the current DPI.
#include "ui/layout.h"

#include "app/strings.h"

static int client_w(const App *a) { return a->m.width; }

static int client_h(const App *a) {
  RECT rc;
  GetClientRect(a->hwnd, &rc);
  return rc.bottom;
}

Box layout_header(const App *a) { return (Box){0, 0, client_w(a), a->m.header_h}; }

Box layout_mark(const App *a) {
  int s = app_px(a, 18);
  return (Box){a->m.pad_x, (a->m.header_h - s) / 2, s, s};
}

Box layout_gear(const App *a) {
  int s = app_px(a, 32);
  return (Box){client_w(a) - app_px(a, 8) - s, (a->m.header_h - s) / 2, s, s};
}

Box layout_back(const App *a) {
  int s = app_px(a, 32);
  return (Box){app_px(a, 8), (a->m.header_h - s) / 2, s, s};
}

Box layout_search(const App *a) {
  Box mark = layout_mark(a), gear = layout_gear(a);
  int x = mark.x + mark.w + app_px(a, 10), h = app_px(a, 22);
  return (Box){x, (a->m.header_h - h) / 2, gear.x - app_px(a, 8) - x, h};
}

Box layout_colhead(const App *a) { return (Box){0, a->m.header_h, client_w(a), a->m.colhead_h}; }

Box layout_body(const App *a) {
  int top = a->m.header_h;
  if (a->view == POPUP_LIST && a->visible_count > 0) top += a->m.colhead_h;
  return (Box){0, top, client_w(a), client_h(a) - a->m.status_h - top};
}

Box layout_status(const App *a) {
  int h = client_h(a);
  return (Box){0, h - a->m.status_h, client_w(a), a->m.status_h};
}

int layout_rows_shown(const App *a) { return a->visible_count < MAX_ROWS ? a->visible_count : MAX_ROWS; }

Box layout_row(const App *a, int vi) {
  int slot = vi - a->top;
  if (vi < 0 || slot < 0 || slot >= layout_rows_shown(a)) return (Box){0};
  Box body = layout_body(a);
  return (Box){0, body.y + slot * a->m.row_h, client_w(a), a->m.row_h};
}

int layout_hit_row(const App *a, int x, int y) {
  if (a->view != POPUP_LIST) return -1;
  for (int slot = 0; slot < layout_rows_shown(a); slot++) {
    int vi = a->top + slot;
    if (box_contains(layout_row(a, vi), x, y)) return vi;
  }
  return -1;
}

// A button of the confirm row: its label and key, measured.
static int confirm_button_w(const App *a, const char *label, const char *key) {
  return (int)(ui_text_width(a->font, label) + ui_text_width(a->font_small, key)) + app_px(a, 22);
}

void layout_row_parts(const App *a, Box row, RowParts *p) {
  const Metrics *m = &a->m;
  int x = row.x + m->pad_x, right = row.x + row.w - m->pad_x;
  int cy = row.y, h = row.h;
  p->port = (Box){x, cy, m->col_port, h};
  p->name = (Box){p->port.x + m->col_port + m->gap, cy, m->col_name, h};
  p->up = (Box){right - m->col_up, cy, m->col_up, h};
  p->tags = (Box){p->up.x - m->gap - m->col_tags, cy, m->col_tags, h};
  int folder_x = p->name.x + m->col_name + m->gap;
  p->folder = (Box){folder_x, cy, p->tags.x - m->gap - folder_x, h};

  int bw = m->button_w, bh = m->button_h, by = cy + (h - bh) / 2, step = bw + app_px(a, 2);
  int bx = row.x + row.w - app_px(a, 8) - bw;
  p->stop = (Box){bx, by, bw, bh};
  p->folder_button = (Box){bx - step, by, bw, bh};
  p->browser = (Box){bx - 2 * step, by, bw, bh};

  int cw = confirm_button_w(a, strings()->stop, "Enter");
  int kw = confirm_button_w(a, strings()->cancel, "Esc");
  p->confirm = (Box){row.x + row.w - app_px(a, 8) - cw, by, cw, bh};
  p->cancel = (Box){p->confirm.x - app_px(a, 4) - kw, by, kw, bh};
  p->text = (Box){p->name.x, cy, p->cancel.x - m->gap - p->name.x, h};
}
