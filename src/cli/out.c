// Writing to and reading from the standard handles, whatever they are.
#include "cli/out.h"

#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "base/utf.h"

static bool is_console(HANDLE h) {
  DWORD mode;
  return GetConsoleMode(h, &mode);
}

static void write_to(DWORD which, const char *utf8) {
  HANDLE h = GetStdHandle(which);
  if (!h || h == INVALID_HANDLE_VALUE || !utf8[0]) return;
  DWORD done;
  if (is_console(h)) {
    wchar_t *w = utf8_to_wide(utf8, -1);
    WriteConsoleW(h, w, (DWORD)wcslen(w), &done, NULL);
    free(w);
  } else {
    WriteFile(h, utf8, (DWORD)strlen(utf8), &done, NULL);
  }
}

void out_text(const char *utf8) { write_to(STD_OUTPUT_HANDLE, utf8); }

void out_line(const char *utf8) {
  write_to(STD_OUTPUT_HANDLE, utf8);
  write_to(STD_OUTPUT_HANDLE, "\n");
}

void err_line(const char *utf8) {
  write_to(STD_ERROR_HANDLE, utf8);
  write_to(STD_ERROR_HANDLE, "\n");
}

static bool console_line(HANDLE h, char *out, size_t cap) {
  wchar_t w[256];
  DWORD n = 0;
  if (!ReadConsoleW(h, w, 255, &n, NULL) || n == 0) return false;
  while (n && (w[n - 1] == L'\n' || w[n - 1] == L'\r')) n--;
  w[n] = 0;
  return WideCharToMultiByte(CP_UTF8, 0, w, -1, out, (int)cap, NULL, NULL) > 0;
}

// Byte by byte, so nothing past the line is taken from the pipe. A UTF-8
// byte order mark in front (PowerShell sends one) is dropped.
static bool byte_line(HANDLE h, char *out, size_t cap) {
  size_t n = 0;
  char c;
  DWORD got;
  bool any = false;
  while (ReadFile(h, &c, 1, &got, NULL) && got == 1) {
    any = true;
    if (c == '\n') break;
    if (c != '\r' && n + 1 < cap) out[n++] = c;
  }
  out[n] = 0;
  if (n >= 3 && memcmp(out, "\xEF\xBB\xBF", 3) == 0) memmove(out, out + 3, n - 2);
  return any;
}

bool in_line(char *out, size_t cap) {
  HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
  out[0] = 0;
  if (!h || h == INVALID_HANDLE_VALUE) return false;
  if (is_console(h)) return console_line(h, out, cap);
  bool got = byte_line(h, out, cap);
  out_text("\n");  // nothing echoed the answer, so end the prompt's line here
  return got;
}
