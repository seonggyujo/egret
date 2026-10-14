// Folder list edits for the user's PATH.
#include "app/path_list.h"

#include <string.h>

// Whether the entry of len characters names dir.
static bool same_dir(const wchar_t *entry, size_t len, const wchar_t *dir) {
  size_t dn = wcslen(dir);
  while (len && entry[len - 1] == L'\\') len--;
  while (dn && dir[dn - 1] == L'\\') dn--;
  return len == dn && len && _wcsnicmp(entry, dir, len) == 0;
}

bool path_list_has(const wchar_t *list, const wchar_t *dir) {
  for (const wchar_t *p = list; p && *p;) {
    const wchar_t *end = wcschr(p, L';');
    size_t len = end ? (size_t)(end - p) : wcslen(p);
    if (same_dir(p, len, dir)) return true;
    p = end ? end + 1 : NULL;
  }
  return false;
}

static bool append(wchar_t *out, size_t cap, size_t *at, const wchar_t *entry, size_t len) {
  if (*at + len + 2 > cap) return false;
  if (*at) out[(*at)++] = L';';
  memcpy(out + *at, entry, len * sizeof *entry);
  *at += len;
  out[*at] = 0;
  return true;
}

bool path_list_with(const wchar_t *list, const wchar_t *dir, bool present, wchar_t *out, size_t cap) {
  size_t at = 0;
  if (cap == 0) return false;
  out[0] = 0;
  for (const wchar_t *p = list; p && *p;) {
    const wchar_t *end = wcschr(p, L';');
    size_t len = end ? (size_t)(end - p) : wcslen(p);
    if (len && !same_dir(p, len, dir) && !append(out, cap, &at, p, len)) return false;
    p = end ? end + 1 : NULL;
  }
  return !present || append(out, cap, &at, dir, wcslen(dir));
}
