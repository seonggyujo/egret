// The list of programs taken off the list, and its one settings value.
#include "app/hidden.h"

#include <string.h>

#include "app/prefs.h"
#include "base/names.h"

bool hidden_contains(const HiddenList *h, const char *exe) {
  for (int i = 0; i < h->count; i++) {
    if (name_equal(h->names[i], exe)) return true;
  }
  return false;
}

static void add_name(HiddenList *h, const char *name, size_t len) {
  char one[128];
  if (len == 0 || len >= sizeof one || h->count == HIDDEN_MAX) return;
  memcpy(one, name, len);
  one[len] = 0;
  if (!hidden_contains(h, one)) strcpy(h->names[h->count++], one);
}

void hidden_parse(HiddenList *h, const char *joined) {
  h->count = 0;
  while (joined && *joined) {
    const char *end = strchr(joined, ';');
    size_t len = end ? (size_t)(end - joined) : strlen(joined);
    while (len && joined[0] == ' ') joined++, len--;
    while (len && joined[len - 1] == ' ') len--;
    add_name(h, joined, len);
    joined = end ? end + 1 : NULL;
  }
}

void hidden_join(const HiddenList *h, char *out, size_t cap) {
  size_t at = 0;
  out[0] = 0;
  for (int i = 0; i < h->count; i++) {
    size_t n = strlen(h->names[i]);
    if (at + n + 2 > cap) break;
    if (at) out[at++] = ';';
    memcpy(out + at, h->names[i], n + 1);
    at += n;
  }
}

void hidden_load(HiddenList *h) {
  char joined[HIDDEN_MAX * 130];
  hidden_parse(h, prefs_get(PREF_HIDDEN, joined, sizeof joined) ? joined : "");
}

static void save(const HiddenList *h) {
  char joined[HIDDEN_MAX * 130];
  hidden_join(h, joined, sizeof joined);
  if (joined[0]) prefs_set(PREF_HIDDEN, joined);
  else prefs_delete(PREF_HIDDEN);
}

void hidden_add(HiddenList *h, const char *exe) {
  add_name(h, exe, strlen(exe));
  save(h);
}

void hidden_remove(HiddenList *h, const char *exe) {
  for (int i = 0; i < h->count; i++) {
    if (!name_equal(h->names[i], exe)) continue;
    memmove(h->names[i], h->names[i + 1], sizeof h->names[0] * (size_t)(h->count - i - 1));
    h->count--;
    break;
  }
  save(h);
}

int hidden_pointers(const HiddenList *h, const char **out, int max) {
  int n = h->count < max ? h->count : max;
  for (int i = 0; i < n; i++) out[i] = h->names[i];
  return n;
}
