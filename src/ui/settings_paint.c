// The settings rows: a label on the left, its control on the right.
#include "ui/settings_paint.h"

#include "app/hotkey.h"
#include "app/strings.h"
#include "app/theme.h"
#include "ui/settings_layout.h"
#include "ui/widgets.h"

static void row_line(App *a, Box row) {
  canvas_fill_rect(a->canvas, (float)row.x, (float)(row.y + row.h - 1), (float)row.w, 1, theme()->line);
}

static void label(App *a, Box row, const char *text) {
  ui_text_draw(a->canvas, a->font, text, (float)row.x, (float)row.y, row.w * 0.55f, (float)row.h, theme()->text,
               ALIGN_LEFT);
  row_line(a, row);
}

static int hovered(const App *a, int first, int n) {
  int i = a->hover_part - first;
  return i >= 0 && i < n ? i : -1;
}

static void paint_hotkey(App *a, Box b) {
  const ThemeColors *t = theme();
  char key[64];
  hotkey_format(a->hotkey_mods, a->hotkey_vk, key, sizeof key);
  bool hover = a->hover_part == PART_HOTKEY, waiting = a->capturing_hotkey;
  canvas_fill_round(a->canvas, (float)b.x, (float)b.y, (float)b.w, (float)b.h, 4 * a->scale,
                    hover || waiting ? t->button_hover : t->button);
  canvas_stroke_round(a->canvas, (float)b.x, (float)b.y, (float)b.w, (float)b.h, 4 * a->scale, 1,
                      waiting ? t->text : t->button_line);
  UiFont *f = waiting ? a->font : a->font_num;
  ui_text_draw(a->canvas, f, waiting ? strings()->hotkey_press : key, (float)b.x, (float)b.y, (float)b.w,
               (float)b.h, a->hotkey_ok || waiting ? t->text : t->error, ALIGN_CENTER);
}

static void paint_hidden(App *a, const SettingsLayout *s) {
  const ThemeColors *t = theme();
  const Strings *str = strings();
  Box r = s->hidden_row;
  ui_text_draw(a->canvas, a->font, str->hidden_programs, (float)r.x, (float)r.y, r.w * 0.6f, (float)r.h, t->text,
               ALIGN_LEFT);
  ui_text_draw(a->canvas, a->font_small, str->hidden_hint, (float)r.x, (float)r.y, (float)r.w, (float)r.h,
               t->text2, ALIGN_RIGHT);
  if (s->chip_count == 0) {
    Box n = s->none;
    ui_text_draw(a->canvas, a->font, str->hidden_none, (float)n.x, (float)n.y, (float)n.w, (float)n.h, t->text2,
                 ALIGN_LEFT);
  }
  for (int i = 0; i < s->chip_count; i++) {
    widget_chip(a, s->chips[i], s->chip_x[i], a->hidden.names[i], a->hover_part == PART_CHIP + i);
  }
}

void settings_paint(App *a) {
  const Strings *str = strings();
  SettingsLayout s;
  settings_layout(a, &s);
  label(a, s.theme_row, str->theme);
  const char *themes[] = {str->theme_system, str->theme_light, str->theme_dark};
  widget_segments(a, s.theme, 3, themes, (int)theme_mode(), hovered(a, PART_THEME, 3));
  label(a, s.lang_row, str->language);
  const char *langs[] = {"한국어", "English"};
  widget_segments(a, s.lang, 2, langs, strings_lang() == LANG_KO ? 0 : 1, hovered(a, PART_LANG, 2));
  label(a, s.hotkey_row, str->hotkey);
  paint_hotkey(a, s.hotkey);
  label(a, s.autostart_row, str->autostart);
  widget_switch(a, s.autostart, a->autostart, a->hover_part == PART_AUTOSTART);
  Box p = s.path_row;
  row_line(a, p);
  ui_text_draw(a->canvas, a->font, str->cli_path, (float)p.x, (float)p.y + app_px(a, 4), p.w * 0.7f,
               (float)app_px(a, 28), theme()->text, ALIGN_LEFT);
  Box h = s.path_hint;
  ui_text_draw(a->canvas, a->font_small, str->cli_path_hint, (float)h.x, (float)h.y, (float)h.w, (float)h.h,
               theme()->text2, ALIGN_LEFT);
  widget_switch(a, s.path, a->user_path, a->hover_part == PART_PATH);
  paint_hidden(a, &s);
}
