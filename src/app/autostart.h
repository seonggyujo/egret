// Starting with Windows: the "egret" value under
// HKCU\Software\Microsoft\Windows\CurrentVersion\Run, set to
// "<folder>\egret.exe" --tray.
#pragma once

#include <stdbool.h>

#define TRAY_FLAG "--tray"

// True when the value is there and points at this egret.exe.
bool autostart_enabled(void);
bool autostart_set(bool on);
