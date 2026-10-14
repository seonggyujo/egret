// The keyboard. In the list the search box keeps the focus and hands over
// the list's keys; in the settings the popup itself has the focus.
//   Up, Down, PageUp, PageDown  move the selection
//   Enter                       opens the selected row in the browser, or confirms a stop
//   Del                         asks to stop the selected row; pressed again, stops it
//   Esc                         cancels the stop, else clears the search, else hides
#pragma once

#include <stdbool.h>

#include "ui/app.h"

// From the search box. True when the key was the list's.
bool keys_list(App *a, UINT vk);
// WM_KEYDOWN and WM_SYSKEYDOWN on the popup. True when handled.
bool keys_popup(App *a, UINT vk);
