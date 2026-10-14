// Tooltips for the icon buttons, the tags (LAN, WSL, heron-c) and a row's
// folder (its full path). One Windows tooltip control is moved from target
// to target as the mouse goes.
#pragma once

#include <stdbool.h>

#include "ui/app.h"

void tooltip_create(App *a);
void tooltip_mouse_move(App *a, int x, int y);
// WM_NOTIFY: hands the tooltip its text. True when it was the tooltip's.
bool tooltip_notify(App *a, LPARAM lp);
