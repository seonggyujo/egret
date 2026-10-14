// The scan thread: a full scan every 2 seconds, and right away when woken
// (the popup opened, something was stopped). Each result goes to the
// window as WM_APP_SCANNED with a ListenerList the window then owns.
#pragma once

#include <windows.h>

void scanner_start(HWND hwnd);
void scanner_wake(void);
// Ends the thread (waits up to 2 s for a scan in progress).
void scanner_stop(void);
