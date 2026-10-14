// The mouse inside the list: which part of which row it is over, clicks on
// rows and their buttons, and scrolling.
#pragma once

#include <stdbool.h>

#include "ui/app.h"

// A PART_* from ui/layout.h; *vi gets the row's visible index.
int list_hit(const App *a, int x, int y, int *vi);
void list_click(App *a, int part, int vi);
// Scrolls by rows (negative is up).
void list_scroll(App *a, int rows);
// Whether the mouse rests on the yellow confirm row (it then does not lapse).
bool list_mouse_on_confirm(const App *a);
// Whether a row shows its buttons: the one under the mouse, else the selected one.
bool list_buttons_on(const App *a, int vi);
