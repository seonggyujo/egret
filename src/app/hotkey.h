// The popup's global shortcut as text ("Ctrl+Alt+P") and as the values
// RegisterHotKey takes: mods from MOD_CONTROL, MOD_ALT, MOD_SHIFT, MOD_WIN
// and a virtual-key code. Keys: A-Z, 0-9, F1-F24, Space, Home, End,
// Insert, Delete, PageUp, PageDown, Up, Down, Left, Right.
#pragma once

#include <stdbool.h>
#include <stddef.h>

#define HOTKEY_DEFAULT "Ctrl+Alt+P"

// Names compare case-insensitively; spaces around '+' are allowed.
bool hotkey_parse(const char *text, unsigned *mods, unsigned *vk);
// Modifiers in the order Ctrl, Alt, Shift, Win.
void hotkey_format(unsigned mods, unsigned vk, char *out, size_t cap);
// At least one of Ctrl, Alt, Win, and a key from the list above.
bool hotkey_valid(unsigned mods, unsigned vk);
