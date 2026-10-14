// ASCII case-insensitive name, text and path checks.
#include "base/names.h"

#include <string.h>

static char lower(char c) { return c >= 'A' && c <= 'Z' ? (char)(c - 'A' + 'a') : c; }

bool name_equal(const char *a, const char *b) {
  while (*a && lower(*a) == lower(*b)) a++, b++;
  return lower(*a) == lower(*b);
}

static bool starts_with(const char *s, const char *prefix, size_t n) {
  for (size_t i = 0; i < n; i++) {
    if (!s[i] || lower(s[i]) != lower(prefix[i])) return false;
  }
  return true;
}

bool text_starts_with(const char *s, const char *prefix) { return starts_with(s, prefix, strlen(prefix)); }

bool text_contains(const char *hay, const char *needle, size_t n) {
  if (n == 0) return true;
  for (; *hay; hay++) {
    if (starts_with(hay, needle, n)) return true;
  }
  return false;
}

bool path_inside(const char *path, const char *dir) {
  size_t n = strlen(dir);
  while (n && dir[n - 1] == '\\') n--;
  if (n == 0 || !starts_with(path, dir, n)) return false;
  return path[n] == 0 || path[n] == '\\';
}

void path_folder(const char *path, char *out, size_t cap) {
  const char *slash = strrchr(path, '\\');
  size_t n = slash ? (size_t)(slash - path) : 0;
  if (n == 2 && path[1] == ':') n = 3;  // "C:\x.exe" lives in "C:\"
  if (n >= cap) n = cap - 1;
  memcpy(out, path, n);
  out[n] = 0;
}

const char *path_file_name(const char *path) {
  const char *slash = strrchr(path, '\\');
  return slash ? slash + 1 : path;
}

void text_copy(char *out, size_t cap, const char *src) {
  size_t n = strlen(src);
  if (n >= cap) {
    n = cap - 1;
    while (n && ((unsigned char)src[n] & 0xC0) == 0x80) n--;  // not inside a character
  }
  memcpy(out, src, n);
  out[n] = 0;
}
