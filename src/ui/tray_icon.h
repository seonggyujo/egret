// Drawing the tray icon: the count in dark digits on a yellow rounded
// square, up to 99; with no servers, the egret mark in the taskbar's ink.
#pragma once

#include <stdbool.h>
#include <windows.h>

// size is the small icon size in pixels at the taskbar's DPI.
HICON tray_icon_make(int count, bool light_taskbar, int size);
