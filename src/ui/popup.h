// Showing and hiding the popup: at the bottom right of the work area of
// the screen the mouse is on, as tall as what it shows (bottom edge kept),
// in a Windows 11 frame with rounded corners and the system shadow. It
// hides when it loses focus, on Esc, or on the shortcut again.
#pragma once

#include <stdbool.h>

#include "ui/app.h"

// DWM corners, border color, dark mode and the shadow margin.
void popup_init_frame(App *a);
// The theme changed: frame colors, the search box, a repaint.
void popup_theme_changed(App *a);

void popup_show(App *a, View view);
void popup_hide(App *a);
// The shortcut and the tray icon: show, or hide when shown.
void popup_toggle(App *a);
// Switches between the list and the settings without hiding.
void popup_set_view(App *a, View view);

// Sizes the window for its content, keeping the bottom edge.
void popup_fit(App *a);

// Window messages.
void popup_activated(App *a, bool active);
void popup_dpi_changed(App *a, UINT dpi);
void popup_resized(App *a);
// TIMER_TICK: lapsed confirm and note, uptimes.
void popup_tick(App *a);
