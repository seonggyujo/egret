// What the list shows of the scanned rows: the filter (search text, show
// all, programs taken off the list) applied, the selection kept on the same
// port and PID across scans, and the tray count kept current.
#pragma once

#include <stdint.h>

#include "ui/app.h"

// WM_APP_SCANNED: takes the new rows (and frees the list holder).
void rows_view_take(App *a, ListenerList *fresh);
// After the search text, show-all or the hidden programs changed.
void rows_view_refilter(App *a);
// The visible index of a row, or -1.
int rows_view_find(const App *a, uint16_t port, uint32_t pid);
// Moves the selection to vi (clamped) and scrolls it into view.
void rows_view_select(App *a, int vi);
// Toggles and saves show-all.
void rows_view_toggle_all(App *a);
// The tray shows how many rows are shown by default; after the hidden
// programs changed.
void rows_view_update_tray(App *a);
