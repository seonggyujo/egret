// Small settings kept in %APPDATA%\egret\settings.ini. A convenience only:
// any failure falls back to the default.
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

// Keys in use.
#define PREF_THEME "theme"        // system | light | dark
#define PREF_LANG "lang"          // ko | en; unset follows Windows
#define PREF_HOTKEY "hotkey"      // "Ctrl+Alt+P"
#define PREF_SHOW_ALL "show_all"  // true | false
#define PREF_HIDDEN "hidden"      // exe names taken off the list, joined by ';'

// Copies the value of key into out. Returns false when it is not set.
bool prefs_get(const char *key, char *out, size_t cap);
void prefs_set(const char *key, const char *value);
void prefs_delete(const char *key);

bool prefs_get_bool(const char *key, bool fallback);
void prefs_set_bool(const char *key, bool value);

// The settings folder (made when missing), for "Open settings folder".
bool prefs_folder(wchar_t *out, size_t cap);
