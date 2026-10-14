// Which rows the list shows: by default everything but system rows and the
// programs the user took off the list; the search text narrows that.
#pragma once

#include <stdbool.h>

#include "model/listener.h"

typedef struct {
  const char *query;          // the search text, UTF-8; NULL or "" matches all
  bool show_all;              // system rows and hidden programs too
  const char *const *hidden;  // exe names taken off the list ("Discord.exe")
  int hidden_count;
} RowFilter;

// Shown when nothing is searched and show_all is off: not KIND_SYSTEM and
// not a hidden program (exe names compare ASCII case-insensitively).
bool row_is_default(const Listener *l, const char *const *hidden, int hidden_count);

// The query matches when the port starts with it, or when the name, exe,
// folder, service or image contains it (ASCII case-insensitive).
bool row_matches(const Listener *l, const char *query);

bool row_visible(const Listener *l, const RowFilter *f);

// For the status bar and the tray: rows shown by default, and the rest.
void rows_count(const ListenerList *rows, const char *const *hidden, int hidden_count, int *shown,
                int *tucked);
