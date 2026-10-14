// Which rows the list shows: the default set, then the search text.
#include "model/filter.h"

#include <stdio.h>
#include <string.h>

#include "base/names.h"

static bool is_hidden(const char *exe, const char *const *hidden, int hidden_count) {
  if (!exe[0]) return false;
  for (int i = 0; i < hidden_count; i++) {
    if (name_equal(exe, hidden[i])) return true;
  }
  return false;
}

bool row_is_default(const Listener *l, const char *const *hidden, int hidden_count) {
  return l->kind != KIND_SYSTEM && !is_hidden(l->exe, hidden, hidden_count);
}

bool row_matches(const Listener *l, const char *query) {
  if (!query) return true;
  while (*query == ' ') query++;
  size_t n = strlen(query);
  while (n && query[n - 1] == ' ') n--;
  if (n == 0) return true;
  char port[8];
  snprintf(port, sizeof port, "%u", (unsigned)l->port);
  if (n <= strlen(port) && strncmp(port, query, n) == 0) return true;
  const char *const fields[] = {l->name, l->exe, l->folder, l->service, l->image};
  for (size_t i = 0; i < sizeof fields / sizeof fields[0]; i++) {
    if (text_contains(fields[i], query, n)) return true;
  }
  return false;
}

bool row_visible(const Listener *l, const RowFilter *f) {
  if (!f->show_all && !row_is_default(l, f->hidden, f->hidden_count)) return false;
  return row_matches(l, f->query);
}

void rows_count(const ListenerList *rows, const char *const *hidden, int hidden_count, int *shown,
                int *tucked) {
  int n = 0;
  for (int i = 0; i < rows->count; i++) {
    if (row_is_default(&rows->items[i], hidden, hidden_count)) n++;
  }
  *shown = n;
  *tucked = rows->count - n;
}
