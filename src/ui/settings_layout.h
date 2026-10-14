// Where the settings view's controls are: theme and language choices, the
// shortcut button, the start-with-Windows and PATH switches, and the chips
// of the programs taken off the list. Its height follows the chips.
#pragma once

#include "app/hidden.h"
#include "ui/app.h"
#include "ui/layout.h"

#define PART_THEME (PART_SETTINGS + 0)  // + ThemeMode
#define PART_LANG (PART_SETTINGS + 10)  // + 0 Korean, 1 English
#define PART_HOTKEY (PART_SETTINGS + 20)
#define PART_AUTOSTART (PART_SETTINGS + 21)
#define PART_PATH (PART_SETTINGS + 22)
#define PART_CHIP (PART_SETTINGS + 100)  // + the chip's index

typedef struct {
  Box theme_row, lang_row, hotkey_row, autostart_row, path_row, hidden_row;
  Box theme[3], lang[2], hotkey, autostart, path, path_hint;
  Box chips[HIDDEN_MAX], chip_x[HIDDEN_MAX], none;
  int chip_count, bottom;
} SettingsLayout;

void settings_layout(const App *a, SettingsLayout *s);
// The popup's height in the settings view.
int settings_height(const App *a);
int settings_hit(const App *a, int x, int y);
