// egret's folder in the Path value of HKCU\Environment.
#include "app/user_path.h"

#include <stdlib.h>
#include <windows.h>

#include "app/exe_path.h"
#include "app/path_list.h"
#include "base/log.h"
#include "base/mem.h"

#define ENV_KEY L"Environment"
#define PATH_CHARS 32768

// The current value and its type (REG_EXPAND_SZ when there is none).
static bool read_path(wchar_t *out, DWORD *type) {
  HKEY key;
  out[0] = 0;
  *type = REG_EXPAND_SZ;
  if (RegOpenKeyExW(HKEY_CURRENT_USER, ENV_KEY, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) return false;
  DWORD size = (PATH_CHARS - 1) * sizeof(wchar_t);
  LSTATUS r = RegQueryValueExW(key, L"Path", NULL, type, (BYTE *)out, &size);
  RegCloseKey(key);
  if (r == ERROR_FILE_NOT_FOUND) {
    *type = REG_EXPAND_SZ;
    return true;
  }
  if (r != ERROR_SUCCESS || (*type != REG_SZ && *type != REG_EXPAND_SZ)) return false;
  out[size / sizeof(wchar_t)] = 0;
  return true;
}

bool user_path_enabled(void) {
  wchar_t dir[MAX_PATH];
  wchar_t *path = xmalloc(PATH_CHARS * sizeof *path);
  DWORD type;
  bool on = exe_path_dir(dir, MAX_PATH) && read_path(path, &type) && path_list_has(path, dir);
  free(path);
  return on;
}

static bool write_path(const wchar_t *value, DWORD type) {
  LSTATUS r = RegSetKeyValueW(HKEY_CURRENT_USER, ENV_KEY, L"Path", type, value,
                              (DWORD)((wcslen(value) + 1) * sizeof(wchar_t)));
  if (r != ERROR_SUCCESS) {
    log_warn("user_path", "could not write Path: %ld", (long)r);
    return false;
  }
  SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment", SMTO_ABORTIFHUNG, 2000, NULL);
  return true;
}

bool user_path_set(bool on) {
  wchar_t dir[MAX_PATH];
  wchar_t *path = xmalloc(PATH_CHARS * sizeof *path), *next = xmalloc(PATH_CHARS * sizeof *next);
  DWORD type;
  bool ok = exe_path_dir(dir, MAX_PATH) && read_path(path, &type);
  if (ok && path_list_has(path, dir) != on) {
    ok = path_list_with(path, dir, on, next, PATH_CHARS) && write_path(next, type);
  }
  free(path);
  free(next);
  return ok;
}
