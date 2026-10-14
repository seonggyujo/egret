// Where egret's two programs are: egret.exe (the tray app, also run
// elevated to stop services) and egret.com (the command line), side by side.
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

// This program's full path.
bool exe_path_self(wchar_t *out, size_t cap);
// The folder holding it, without a trailing backslash.
bool exe_path_dir(wchar_t *out, size_t cap);
// egret.exe in that folder.
bool exe_path_gui(wchar_t *out, size_t cap);
