// One frame of the popup.
#include "ui/paint.h"

#include "app/strings.h"
#include "app/theme.h"
#include "ui/icons.h"
#include "ui/layout.h"
#include "ui/list_paint.h"
#include "ui/settings_paint.h"
#include "ui/status.h"
#include "ui/widgets.h"

static void paint_header(App *a) {
  const ThemeColors *t = theme();
  Box h = layout_header(a);
  canvas_fill_rect(a->canvas, 0, 0, (float)h.w, (float)h.h, t->header);
  canvas_fill_rect(a->canvas, 0, (float)(h.h - 1), (float)h.w, 1, t->header_line);
  if (a->view == POPUP_LIST) {
    Box m = layout_mark(a);
    icon_mark(a->canvas, (float)m.x, (float)m.y, (float)m.w, t->mark, t->bill);
    widget_ghost_button(a, layout_gear(a), ICON_GEAR, a->hover_part == PART_GEAR);
    return;
  }
  Box back = layout_back(a);
  widget_ghost_button(a, back, ICON_BACK, a->hover_part == PART_BACK);
  float x = (float)(back.x + back.w + app_px(a, 6));
  ui_text_draw(a->canvas, a->font_title, strings()->settings, x, 0, (float)h.w - x, (float)h.h, t->text,
               ALIGN_LEFT);
}

void paint(App *a) {
  PAINTSTRUCT ps;
  HDC dc = BeginPaint(a->hwnd, &ps);
  RECT rc;
  GetClientRect(a->hwnd, &rc);
  if (canvas_begin(a->canvas, dc, rc.right, rc.bottom)) {
    canvas_clear(a->canvas, theme()->bg);
    paint_header(a);
    if (a->view == POPUP_LIST) list_paint(a);
    else settings_paint(a);
    status_paint(a);
    canvas_end(a->canvas);
  }
  EndPaint(a->hwnd, &ps);
}
