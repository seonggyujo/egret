// Drawing the shared controls (sizes from the design mockup).
#include "ui/widgets.h"

#include "app/theme.h"

static void fill(App *a, Box b, float r, uint32_t color) {
  canvas_fill_round(a->canvas, (float)b.x, (float)b.y, (float)b.w, (float)b.h, r, color);
}

static void stroke(App *a, Box b, float r, uint32_t color) {
  canvas_stroke_round(a->canvas, (float)b.x, (float)b.y, (float)b.w, (float)b.h, r, 1, color);
}

void widget_icon_button(App *a, Box b, IconId icon, bool hover) {
  const ThemeColors *t = theme();
  float r = 4 * a->scale;
  fill(a, b, r, hover ? t->button_hover : t->button);
  stroke(a, b, r, t->button_line);
  icon_draw(a->canvas, icon, b, 14 * a->scale, t->text);
}

void widget_ghost_button(App *a, Box b, IconId icon, bool hover) {
  const ThemeColors *t = theme();
  if (hover) fill(a, b, 4 * a->scale, t->hover);
  icon_draw(a->canvas, icon, b, 16 * a->scale, hover ? t->text : t->text2);
}

void widget_text_button(App *a, Box b, const char *label, const char *key, uint32_t fill_color, uint32_t line,
                        uint32_t color, uint32_t key_color) {
  float r = 4 * a->scale;
  if (fill_color) fill(a, b, r, fill_color);
  stroke(a, b, r, line);
  float lw = ui_text_width(a->font, label), gap = 5 * a->scale;
  float kw = ui_text_width(a->font_small, key);
  float x = b.x + (b.w - lw - gap - kw) / 2;
  ui_text_draw(a->canvas, a->font, label, x, (float)b.y, lw + 1, (float)b.h, color, ALIGN_LEFT);
  ui_text_draw(a->canvas, a->font_small, key, x + lw + gap, (float)b.y, kw + 1, (float)b.h, key_color, ALIGN_LEFT);
}

void widget_switch(App *a, Box b, bool on, bool hover) {
  const ThemeColors *t = theme();
  float r = b.h / 2.0f, knob = 12 * a->scale, pad = (b.h - knob) / 2;
  if (on) {
    fill(a, b, r, t->switch_on);
  } else {
    fill(a, b, r, hover ? t->hover : t->button);
    stroke(a, b, r, t->dim);
  }
  float cx = on ? b.x + b.w - pad - knob / 2 : b.x + pad + knob / 2;
  canvas_fill_circle(a->canvas, cx, b.y + b.h / 2.0f, knob / 2, on ? t->switch_knob : t->dim);
}

void widget_segments(App *a, const Box *boxes, int n, const char *const *labels, int on, int hover) {
  const ThemeColors *t = theme();
  int pad = app_px(a, 2);
  Box all = {boxes[0].x - pad, boxes[0].y - pad, boxes[n - 1].x + boxes[n - 1].w - boxes[0].x + 2 * pad,
             boxes[0].h + 2 * pad};
  fill(a, all, 5 * a->scale, t->segment_bg);
  for (int i = 0; i < n; i++) {
    Box b = boxes[i];
    if (i == on) {
      fill(a, b, 4 * a->scale, t->segment_on);
      stroke(a, b, 4 * a->scale, t->button_line);
    } else if (i == hover) {
      fill(a, b, 4 * a->scale, t->hover);
    }
    ui_text_draw(a->canvas, i == on ? a->font_tag : a->font, labels[i], (float)b.x, (float)b.y, (float)b.w,
                 (float)b.h, i == on ? t->text : t->text2, ALIGN_CENTER);
  }
}

void widget_chip(App *a, Box b, Box x, const char *text, bool hover_x) {
  const ThemeColors *t = theme();
  fill(a, b, 4 * a->scale, t->button);
  stroke(a, b, 4 * a->scale, t->button_line);
  int pad = app_px(a, 8);
  ui_text_draw(a->canvas, a->font, text, (float)(b.x + pad), (float)b.y, (float)(x.x - b.x - pad), (float)b.h,
               t->text, ALIGN_LEFT);
  if (hover_x) fill(a, x, 3 * a->scale, t->hover);
  icon_draw(a->canvas, ICON_CLOSE, x, 10 * a->scale, hover_x ? t->text : t->text2);
}

void widget_link(App *a, Box b, const char *text, bool hover, TextAlign align) {
  const ThemeColors *t = theme();
  ui_text_draw(a->canvas, a->font_small, text, (float)b.x, (float)b.y, (float)b.w, (float)b.h, t->text, align);
  float w = ui_text_width(a->font_small, text);
  float x = align == ALIGN_RIGHT ? b.x + b.w - w : align == ALIGN_CENTER ? b.x + (b.w - w) / 2 : (float)b.x;
  float y = b.y + b.h / 2.0f + 7 * a->scale;
  canvas_fill_rect(a->canvas, x, y, w, hover ? 1.5f * a->scale : 1, hover ? t->text : t->text2);
}
