// The Run registry value that starts egret with Windows.
#include "app/autostart.h"

#include <stdio.h>
#include <wchar.h>
#include <windows.h>

#include "app/exe_path.h"
#include "base/log.h"

#define RUN_KEY L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define VALUE_NAME L"egret"

// "\"C:\...\egret.exe\" --tray".
static bool command(wchar_t *out, size_t cap) {
  wchar_t exe[MAX_PATH];
  if (!exe_path_gui(exe, MAX_PATH)) return false;
  return swprintf(out, cap, L"\"%ls\" %hs", exe, TRAY_FLAG) > 0;
}

bool autostart_enabled(void) {
  wchar_t want[MAX_PATH + 16], have[MAX_PATH + 16];
  DWORD size = sizeof have;
  if (!command(want, MAX_PATH + 16)) return false;
  if (RegGetValueW(HKEY_CURRENT_USER, RUN_KEY, VALUE_NAME, RRF_RT_REG_SZ, NULL, have, &size) != ERROR_SUCCESS) {
    return false;
  }
  return _wcsicmp(want, have) == 0;
}

bool autostart_set(bool on) {
  LSTATUS r;
  if (on) {
    wchar_t cmd[MAX_PATH + 16];
    if (!command(cmd, MAX_PATH + 16)) return false;
    r = RegSetKeyValueW(HKEY_CURRENT_USER, RUN_KEY, VALUE_NAME, REG_SZ, cmd,
                        (DWORD)((wcslen(cmd) + 1) * sizeof(wchar_t)));
  } else {
    r = RegDeleteKeyValueW(HKEY_CURRENT_USER, RUN_KEY, VALUE_NAME);
    if (r == ERROR_FILE_NOT_FOUND) r = ERROR_SUCCESS;
  }
  if (r != ERROR_SUCCESS) log_warn("autostart", "could not %s: %ld", on ? "set" : "remove", (long)r);
  return r == ERROR_SUCCESS;
}
