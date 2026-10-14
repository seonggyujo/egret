// The scan thread and its wake-up event.
#include "ui/scanner.h"

#include <stdlib.h>

#include "base/log.h"
#include "base/mem.h"
#include "scan/scan.h"
#include "ui/app.h"

#define PERIOD_MS 2000

static HANDLE g_thread, g_wake;
static volatile LONG g_quit;
static HWND g_hwnd;

static DWORD WINAPI scan_loop(void *unused) {
  while (!g_quit) {
    ListenerList *rows = xcalloc(1, sizeof *rows);
    scan_run(rows);
    if (g_quit || !PostMessageW(g_hwnd, WM_APP_SCANNED, 0, (LPARAM)rows)) {
      listener_list_free(rows);
      free(rows);
    }
    WaitForSingleObject(g_wake, PERIOD_MS);
  }
  return 0;
}

void scanner_start(HWND hwnd) {
  g_hwnd = hwnd;
  g_wake = CreateEventW(NULL, FALSE, FALSE, NULL);
  g_thread = CreateThread(NULL, 0, scan_loop, NULL, 0, NULL);
  if (!g_thread) log_error("scanner", "CreateThread failed: %lu", GetLastError());
}

void scanner_wake(void) {
  if (g_wake) SetEvent(g_wake);
}

void scanner_stop(void) {
  if (!g_thread) return;
  InterlockedExchange(&g_quit, 1);
  SetEvent(g_wake);
  WaitForSingleObject(g_thread, 2000);
  CloseHandle(g_thread);
  g_thread = NULL;
}
