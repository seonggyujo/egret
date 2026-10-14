// The popup window: its class, creating it together with the tray icon,
// the shortcut and the scan thread, and the switch that hands each window
// message to the file that handles it.
#pragma once

#include <stdbool.h>
#include <windows.h>

// show_popup: open the popup right away (egret was started by hand).
bool window_create(HINSTANCE instance, bool show_popup);
