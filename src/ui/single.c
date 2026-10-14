// A named mutex marks the running egret; its window is found by class.
#include "ui/single.h"

#include <windows.h>

#include "base/log.h"
#include "ui/app.h"

#ifdef EGRET_DEV
#define MUTEX_NAME L"Local\\egret.tray.dev"
#else
#define MUTEX_NAME L"Local\\egret.tray"
#endif
#define WAIT_MS 2000  // for a first launch that has no window yet

bool single_hand_over(bool show) {
  // Held until the process ends.
  CreateMutexW(NULL, FALSE, MUTEX_NAME);
  if (GetLastError() != ERROR_ALREADY_EXISTS) return false;
  HWND hwnd = NULL;
  for (int waited = 0; !hwnd && waited < WAIT_MS; waited += 50) {
    hwnd = FindWindowW(SINGLE_CLASS, NULL);
    if (!hwnd) Sleep(50);
  }
  if (!hwnd || !show) return true;
  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  AllowSetForegroundWindow(pid);  // this launch is in front, so it may hand that over
  PostMessageW(hwnd, WM_APP_SHOW, 0, 0);
  log_info("single", "egret already runs; asked it to show");
  return true;
}
