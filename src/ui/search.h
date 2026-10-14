// The search box at the top of the list: a borderless Windows edit control
// (so typing, the IME, selection and paste just work) in the theme's colors.
// It keeps the focus in the list view, so it also passes the list's keys
// (arrows, Enter, Del, Esc) on to ui/keys.h.
#pragma once

#include "ui/app.h"

void search_create(App *a);
// Position, font and visibility for the current view and DPI.
void search_place(App *a);
void search_clear(App *a);
// EN_CHANGE: the new text filters the list.
void search_changed(App *a);
// WM_CTLCOLOREDIT: returns the background brush.
LRESULT search_colors(App *a, HDC dc);
void search_theme_changed(App *a);
// The language changed: the hint text.
void search_language_changed(App *a);
