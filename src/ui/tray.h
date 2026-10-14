// The notification area icon: the server count on the bill's yellow, or
// the egret mark when nothing listens; its tooltip; a left click toggles the
// popup and a right click opens the menu (open, system ports, settings,
// quit). Notifications for stops that finish after the popup closed.
#pragma once

#include "ui/app.h"

void tray_add(App *a);
void tray_remove(App *a);
// Draws the icon again (the taskbar's theme or the DPI changed).
void tray_refresh(App *a);
// The count of servers shown by default; redraws only when it changed.
void tray_set_count(App *a, int count);
// WM_APP_TRAY.
void tray_event(App *a, WPARAM wp, LPARAM lp);
// A Windows notification from the tray icon.
void tray_notify(App *a, const char *text);
