// Small controls drawn in the theme, shared by the list and the settings:
// icon buttons, text buttons with a key hint, the switch, segmented
// choices, removable chips and text links. Drawing only; callers hit-test
// the same boxes they drew.
#pragma once

#include <stdbool.h>

#include "render/ui_text.h"
#include "ui/app.h"
#include "ui/icons.h"

void widget_icon_button(App *a, Box b, IconId icon, bool hover);
// A plain icon button without a frame (the gear, back).
void widget_ghost_button(App *a, Box b, IconId icon, bool hover);
// "label key" centered; key in key_color.
void widget_text_button(App *a, Box b, const char *label, const char *key, uint32_t fill, uint32_t line,
                        uint32_t color, uint32_t key_color);
void widget_switch(App *a, Box b, bool on, bool hover);
// n choices side by side in one rounded group; on is the chosen one.
void widget_segments(App *a, const Box *boxes, int n, const char *const *labels, int on, int hover);
// A program name with an x to remove it.
void widget_chip(App *a, Box b, Box x, const char *text, bool hover_x);
// Underlined text.
void widget_link(App *a, Box b, const char *text, bool hover, TextAlign align);
