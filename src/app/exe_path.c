// This program's path, its folder, and egret.exe beside it.
#include "app/exe_path.h"

#include <string.h>
#include <windows.h>

bool exe_path_self(wchar_t *out, size_t cap) {
  DWORD n = GetModuleFileNameW(NULL, out, (DWORD)cap);
  return n > 0 && n < cap;
}

bool exe_path_dir(wchar_t *out, size_t cap) {
  if (!exe_path_self(out, cap)) return false;
  wchar_t *slash = wcsrchr(out, L'\\');
  if (!slash) return false;
  *slash = 0;
  return true;
}

bool exe_path_gui(wchar_t *out, size_t cap) {
  if (!exe_path_dir(out, cap) || wcslen(out) + 11 > cap) return false;
  wcscat(out, L"\\egret.exe");
  return true;
}
