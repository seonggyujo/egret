// What the settings' controls do when clicked, and taking a new shortcut
// from the keyboard.
#pragma once

#include <stdbool.h>

#include "ui/app.h"

// Entering the view: reads the start-with-Windows and PATH states.
void settings_open(App *a);
void settings_click(App *a, int part);
// While waiting for a new shortcut: true when the key was taken.
bool settings_capture_key(App *a, UINT vk);
