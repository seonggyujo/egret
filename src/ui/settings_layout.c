// The settings view's boxes, measured with the current fonts.
#include "ui/settings_layout.h"

#include "app/hotkey.h"
#include "app/strings.h"

// Choices laid out right to left so the group ends at right.
static void segments(const App *a, Box row, const char *const *labels, int n, int right, Box *out) {
  int h = app_px(a, 26), y = row.y + (row.h - h) / 2, x = right - app_px(a, 2);
  for (int i = n - 1; i >= 0; i--) {
    int w = (int)ui_text_width(a->font_tag, labels[i]) + app_px(a, 24);
    x -= w;
    out[i] = (Box){x, y, w, h};
    x -= app_px(a, 2);
  }
}

static void chips(const App *a, SettingsLayout *s, int left, int right, int y) {
  int h = app_px(a, 24), gap = app_px(a, 6), x = left, xw = app_px(a, 18);
  s->chip_count = a->hidden.count;
  for (int i = 0; i < a->hidden.count; i++) {
    int w = (int)ui_text_width(a->font, a->hidden.names[i]) + app_px(a, 8 + 6 + 4) + xw;
    if (x > left && x + w > right) {
      x = left;
      y += h + gap;
    }
    s->chips[i] = (Box){x, y, w, h};
    s->chip_x[i] = (Box){x + w - app_px(a, 4) - xw, y + (h - xw) / 2, xw, xw};
    x += w + gap;
  }
  s->none = (Box){left, y, right - left, h};
  s->bottom = y + h + app_px(a, 16);
}

void settings_layout(const App *a, SettingsLayout *s) {
  const Strings *t = strings();
  int left = app_px(a, 18), right = a->m.width - left, row = app_px(a, 44), w = right - left;
  int y = a->m.header_h + app_px(a, 4);
  s->theme_row = (Box){left, y, w, row};
  s->lang_row = (Box){left, y += row, w, row};
  s->hotkey_row = (Box){left, y += row, w, row};
  s->autostart_row = (Box){left, y += row, w, row};
  s->path_row = (Box){left, y += row, w, app_px(a, 58)};
  s->hidden_row = (Box){left, y += s->path_row.h, w, app_px(a, 34)};
  const char *themes[] = {t->theme_system, t->theme_light, t->theme_dark};
  const char *langs[] = {"한국어", "English"};
  segments(a, s->theme_row, themes, 3, right, s->theme);
  segments(a, s->lang_row, langs, 2, right, s->lang);
  char key[64];
  hotkey_format(a->hotkey_mods, a->hotkey_vk, key, sizeof key);
  const char *shown = a->capturing_hotkey ? t->hotkey_press : key;
  int hw = (int)ui_text_width(a->font_num, shown) + app_px(a, 20), hh = app_px(a, 28);
  s->hotkey = (Box){right - hw, s->hotkey_row.y + (row - hh) / 2, hw, hh};
  int sw = app_px(a, 40), sh = app_px(a, 20);
  s->autostart = (Box){right - sw, s->autostart_row.y + (row - sh) / 2, sw, sh};
  s->path = (Box){right - sw, s->path_row.y + app_px(a, 12), sw, sh};
  s->path_hint = (Box){left, s->path_row.y + app_px(a, 30), w - sw - app_px(a, 12), app_px(a, 18)};
  chips(a, s, left, right, s->hidden_row.y + s->hidden_row.h);
}

int settings_height(const App *a) {
  SettingsLayout s;
  settings_layout(a, &s);
  return s.bottom + a->m.status_h;
}

int settings_hit(const App *a, int x, int y) {
  SettingsLayout s;
  settings_layout(a, &s);
  for (int i = 0; i < 3; i++) {
    if (box_contains(s.theme[i], x, y)) return PART_THEME + i;
  }
  for (int i = 0; i < 2; i++) {
    if (box_contains(s.lang[i], x, y)) return PART_LANG + i;
  }
  if (box_contains(s.hotkey, x, y)) return PART_HOTKEY;
  if (box_contains(s.autostart, x, y)) return PART_AUTOSTART;
  if (box_contains(s.path, x, y)) return PART_PATH;
  for (int i = 0; i < s.chip_count; i++) {
    if (box_contains(s.chip_x[i], x, y)) return PART_CHIP + i;
  }
  return PART_NONE;
}
