// Leveled log lines on stderr, safe to call from any thread.
#include "base/log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "base/utf.h"

static const char *const LEVEL_NAMES[] = {"error", "warn", "info", "debug", "trace"};

#ifdef EGRET_DEV
static LogLevel g_level = LOG_DEBUG;
#else
static LogLevel g_level = LOG_WARN;
#endif

void log_init(void) {
  const char *env = getenv("EGRET_LOG");
  if (!env) return;
  for (int i = LOG_ERROR; i <= LOG_TRACE; i++) {
    if (_stricmp(env, LEVEL_NAMES[i]) == 0) g_level = (LogLevel)i;
  }
}

void log_set_level(LogLevel level) { g_level = level; }

int log_enabled(LogLevel level) { return level <= g_level; }

// A console gets UTF-16 so Korean shows whatever its code page is. A pipe or
// file gets the UTF-8 bytes as they are.
static void emit(const char *line, int len) {
  HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
  if (!h || h == INVALID_HANDLE_VALUE) return;
  DWORD mode, written;
  if (GetConsoleMode(h, &mode)) {
    wchar_t *w = utf8_to_wide(line, len);
    WriteConsoleW(h, w, (DWORD)wcslen(w), &written, NULL);
    free(w);
  } else {
    WriteFile(h, line, (DWORD)len, &written, NULL);
  }
}

void log_write(LogLevel level, const char *scope, const char *fmt, ...) {
  if (!log_enabled(level)) return;
  char line[2048];
  SYSTEMTIME t;
  GetLocalTime(&t);
  int n = snprintf(line, sizeof line, "%02u:%02u:%02u.%03u %-5s %s: ", t.wHour, t.wMinute,
                   t.wSecond, t.wMilliseconds, LEVEL_NAMES[level], scope);
  va_list ap;
  va_start(ap, fmt);
  int m = vsnprintf(line + n, sizeof line - (size_t)n - 1, fmt, ap);
  va_end(ap);
  int len = n + (m < 0 ? 0 : m);
  if (len > (int)sizeof line - 2) len = (int)sizeof line - 2;
  line[len++] = '\n';
  emit(line, len);
}
