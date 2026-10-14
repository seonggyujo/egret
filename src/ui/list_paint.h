// Drawing the list view's body: the column titles and the rows, or the
// empty state ("nothing is listening", or no match for the search).
#pragma once

#include "ui/app.h"

void list_paint(App *a);
// row_paint.c: one row at its box (normal, under the mouse, selected,
// yellow confirm, or being stopped).
void row_paint(App *a, int visible_index, Box row);
