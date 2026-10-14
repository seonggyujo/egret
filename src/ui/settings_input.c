// Applying the settings to the running app.
#include "ui/settings_input.h"

#include "app/autostart.h"
#include "app/hotkey.h"
#include "app/strings.h"
#include "app/theme.h"
#include "app/user_path.h"
#include "ui/hotkey_reg.h"
#include "ui/popup.h"
#include "ui/rows_view.h"
#include "ui/search.h"
#include "ui/settings_layout.h"
#include "ui/tray.h"

void settings_open(App *a) {
  a->autostart = autostart_enabled();
  a->user_path = user_path_enabled();
}

static void set_language(App *a, Lang lang) {
  strings_set_lang(lang);
  search_language_changed(a);
  tray_refresh(a);
  popup_fit(a);
}

static void toggle_capture(App *a) {
  a->capturing_hotkey = !a->capturing_hotkey;
  if (a->capturing_hotkey) hotkey_reg_pause(a);
  else hotkey_reg_resume(a);
  SetFocus(a->hwnd);
  InvalidateRect(a->hwnd, NULL, FALSE);
}

void settings_click(App *a, int part) {
  if (part >= PART_THEME && part < PART_THEME + 3) {
    theme_set_mode((ThemeMode)(part - PART_THEME));
    popup_theme_changed(a);
  } else if (part == PART_LANG || part == PART_LANG + 1) {
    set_language(a, part == PART_LANG ? LANG_KO : LANG_EN);
  } else if (part == PART_HOTKEY) {
    toggle_capture(a);
  } else if (part == PART_AUTOSTART) {
    autostart_set(!a->autostart);
    a->autostart = autostart_enabled();
  } else if (part == PART_PATH) {
    user_path_set(!a->user_path);
    a->user_path = user_path_enabled();
  } else if (part >= PART_CHIP && part - PART_CHIP < a->hidden.count) {
    hidden_remove(&a->hidden, a->hidden.names[part - PART_CHIP]);
    rows_view_refilter(a);
    rows_view_update_tray(a);
    popup_fit(a);
  }
  InvalidateRect(a->hwnd, NULL, FALSE);
}

static bool is_modifier(UINT vk) {
  return vk == VK_CONTROL || vk == VK_LCONTROL || vk == VK_RCONTROL || vk == VK_MENU || vk == VK_LMENU ||
         vk == VK_RMENU || vk == VK_SHIFT || vk == VK_LSHIFT || vk == VK_RSHIFT || vk == VK_LWIN || vk == VK_RWIN;
}

static unsigned held_modifiers(void) {
  unsigned mods = 0;
  if (GetKeyState(VK_CONTROL) < 0) mods |= MOD_CONTROL;
  if (GetKeyState(VK_MENU) < 0) mods |= MOD_ALT;
  if (GetKeyState(VK_SHIFT) < 0) mods |= MOD_SHIFT;
  if (GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0) mods |= MOD_WIN;
  return mods;
}

bool settings_capture_key(App *a, UINT vk) {
  if (!a->capturing_hotkey) return false;
  unsigned mods = held_modifiers();
  if (vk == VK_ESCAPE && mods == 0) {
    toggle_capture(a);
    return true;
  }
  if (is_modifier(vk) || !hotkey_valid(mods, vk)) return true;  // keep waiting
  a->capturing_hotkey = false;
  if (!hotkey_reg_set(a, mods, vk)) {
    char key[64], note[160];
    hotkey_format(mods, vk, key, sizeof key);
    format_hotkey_taken(note, sizeof note, key);
    app_note(a, note, true);
  }
  popup_fit(a);
  return true;
}
