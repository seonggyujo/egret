// Growing and freeing a list of rows.
#include "model/listener.h"

#include <stdlib.h>
#include <string.h>

#include "base/mem.h"

static void reserve(ListenerList *l, int count) {
  if (count <= l->cap) return;
  int cap = l->cap ? l->cap : 16;
  while (cap < count) cap *= 2;
  l->items = xrealloc(l->items, sizeof *l->items * (size_t)cap);
  l->cap = cap;
}

Listener *listener_list_add(ListenerList *l) {
  reserve(l, l->count + 1);
  Listener *row = &l->items[l->count++];
  memset(row, 0, sizeof *row);
  return row;
}

void listener_list_free(ListenerList *l) {
  free(l->items);
  l->items = NULL;
  l->count = l->cap = 0;
}
