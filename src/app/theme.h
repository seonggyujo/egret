// The popup's colors, 0xAARRGGBB, in a light and a dark palette close to
// Windows 11's own. The theme follows Windows' app mode unless the user
// picked one. egret's own marks are the yellow of the bill: the confirm row,
// the tray count and the mark's bill.
#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum { THEME_SYSTEM, THEME_LIGHT, THEME_DARK } ThemeMode;

typedef struct {
  uint32_t bg, header, header_line, line, hover, selected, status_bg, window_border;
  uint32_t text, text2, dim;  // main text, secondary text, system rows
  uint32_t button, button_line, button_hover, scroll;
  uint32_t strike, strike_line, strike_text2;  // the yellow confirm row
  uint32_t solid, solid_text;                  // its dark "Stop" button
  uint32_t mark, bill;                         // the egret mark
  uint32_t heron, docker, lan, error;          // tag and message colors
  uint32_t switch_on, switch_knob, segment_bg, segment_on;
} ThemeColors;

const ThemeColors *theme(void);
bool theme_dark(void);
ThemeMode theme_mode(void);
// Saves the choice.
void theme_set_mode(ThemeMode mode);
// Windows' app mode may have changed (WM_SETTINGCHANGE "ImmersiveColorSet").
void theme_system_changed(void);
// The taskbar's mode, which the tray icon follows. Not the app mode.
bool theme_taskbar_light(void);
