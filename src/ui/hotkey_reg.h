// The global shortcut that toggles the popup: registered from the saved
// text (Ctrl+Alt+P by default), changed from the settings, and paused
// while the settings wait for a new combination.
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "ui/app.h"

// Registers the saved shortcut; a tray notification when another program
// holds it.
void hotkey_reg_load(App *a);
// Tries mods+vk; on success it replaces and saves the shortcut, else the
// old one stays registered.
bool hotkey_reg_set(App *a, unsigned mods, unsigned vk);
void hotkey_reg_pause(App *a);
void hotkey_reg_resume(App *a);
void hotkey_reg_text(const App *a, char *out, size_t cap);
