// Settings in an ini file under %APPDATA%\egret.
#include "app/prefs.h"

#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "base/log.h"
#include "base/utf.h"

#define SECTION L"egret"

bool prefs_folder(wchar_t *out, size_t cap) {
  DWORD n = GetEnvironmentVariableW(L"APPDATA", out, (DWORD)cap);
  if (n == 0 || n + 8 >= cap) return false;
  wcscat(out, L"\\egret");
  CreateDirectoryW(out, NULL);
  return true;
}

static bool settings_path(wchar_t *out) {
  if (!prefs_folder(out, MAX_PATH - 16)) return false;
  wcscat(out, L"\\settings.ini");
  return true;
}

bool prefs_get(const char *key, char *out, size_t cap) {
  wchar_t path[MAX_PATH], value[2048];
  if (!settings_path(path)) return false;
  wchar_t *wkey = utf8_to_wide(key, -1);
  DWORD n = GetPrivateProfileStringW(SECTION, wkey, L"", value, 2048, path);
  free(wkey);
  if (n == 0) return false;
  if (!WideCharToMultiByte(CP_UTF8, 0, value, -1, out, (int)cap, NULL, NULL)) return false;
  return true;
}

static void write_value(const char *key, const wchar_t *value) {
  wchar_t path[MAX_PATH];
  if (!settings_path(path)) return;
  wchar_t *wkey = utf8_to_wide(key, -1);
  if (!WritePrivateProfileStringW(SECTION, wkey, value, path)) {
    log_warn("prefs", "could not save %s: error %lu", key, GetLastError());
  }
  free(wkey);
}

void prefs_set(const char *key, const char *value) {
  wchar_t *wvalue = utf8_to_wide(value, -1);
  write_value(key, wvalue);
  free(wvalue);
  log_debug("prefs", "%s = %s", key, value);
}

void prefs_delete(const char *key) { write_value(key, NULL); }

bool prefs_get_bool(const char *key, bool fallback) {
  char v[8];
  if (!prefs_get(key, v, sizeof v)) return fallback;
  return strcmp(v, "true") == 0 ? true : strcmp(v, "false") == 0 ? false : fallback;
}

void prefs_set_bool(const char *key, bool value) { prefs_set(key, value ? "true" : "false"); }
