// RegisterHotKey with the shortcut from the settings.
#include "ui/hotkey_reg.h"

#include "app/hotkey.h"
#include "app/prefs.h"
#include "app/strings.h"
#include "base/log.h"
#include "ui/tray.h"

#define HOTKEY_ID 1

static bool register_keys(App *a, unsigned mods, unsigned vk) {
  return RegisterHotKey(a->hwnd, HOTKEY_ID, mods | MOD_NOREPEAT, vk) != 0;
}

void hotkey_reg_text(const App *a, char *out, size_t cap) { hotkey_format(a->hotkey_mods, a->hotkey_vk, out, cap); }

void hotkey_reg_load(App *a) {
  char saved[64];
  if (!prefs_get(PREF_HOTKEY, saved, sizeof saved) ||
      !hotkey_parse(saved, &a->hotkey_mods, &a->hotkey_vk)) {
    hotkey_parse(HOTKEY_DEFAULT, &a->hotkey_mods, &a->hotkey_vk);
  }
  a->hotkey_ok = register_keys(a, a->hotkey_mods, a->hotkey_vk);
  if (a->hotkey_ok) return;
  char text[64], note[160];
  hotkey_reg_text(a, text, sizeof text);
  log_warn("hotkey", "could not register %s: %lu", text, GetLastError());
  format_hotkey_taken(note, sizeof note, text);
  tray_notify(a, note);
}

bool hotkey_reg_set(App *a, unsigned mods, unsigned vk) {
  UnregisterHotKey(a->hwnd, HOTKEY_ID);
  if (!register_keys(a, mods, vk)) {
    a->hotkey_ok = register_keys(a, a->hotkey_mods, a->hotkey_vk);
    return false;
  }
  a->hotkey_mods = mods;
  a->hotkey_vk = vk;
  a->hotkey_ok = true;
  char text[64];
  hotkey_reg_text(a, text, sizeof text);
  prefs_set(PREF_HOTKEY, text);
  return true;
}

void hotkey_reg_pause(App *a) { UnregisterHotKey(a->hwnd, HOTKEY_ID); }

void hotkey_reg_resume(App *a) { a->hotkey_ok = register_keys(a, a->hotkey_mods, a->hotkey_vk); }
