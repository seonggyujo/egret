// Opening a port in the browser and a folder in File Explorer.
#include "actions/open.h"

#include <stdlib.h>
#include <wchar.h>
#include <windows.h>
// After windows.h, which defines what shellapi.h uses.
#include <shellapi.h>

#include "base/log.h"
#include "base/utf.h"

static bool shell_open(const wchar_t *what) {
  INT_PTR r = (INT_PTR)ShellExecuteW(NULL, L"open", what, NULL, NULL, SW_SHOWNORMAL);
  if (r <= 32) log_warn("open", "ShellExecute failed: %d", (int)r);
  return r > 32;
}

bool open_browser(uint16_t port) {
  wchar_t url[40];
  swprintf(url, 40, L"http://localhost:%u", (unsigned)port);
  return shell_open(url);
}

bool open_folder(const char *folder) {
  if (!folder || !folder[0]) return false;
  wchar_t *w = utf8_to_wide(folder, -1);
  DWORD attrs = GetFileAttributesW(w);
  bool ok = attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY) && shell_open(w);
  free(w);
  return ok;
}
