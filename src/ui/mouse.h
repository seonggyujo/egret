// The mouse: what it is over (for hover colors and tooltips) and what its
// clicks do, in the header, the list, the settings and the status bar.
#pragma once

#include "ui/app.h"

void mouse_move(App *a, int x, int y);
void mouse_leave(App *a);
void mouse_click(App *a, int x, int y);
void mouse_double_click(App *a, int x, int y);
void mouse_right_click(App *a, int x, int y);
void mouse_wheel(App *a, int delta);
