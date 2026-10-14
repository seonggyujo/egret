// Uptime and short path texts.
#include "model/text.h"

#include <stdio.h>
#include <string.h>

#include "base/names.h"

#define TICKS_PER_SECOND 10000000ULL

void text_uptime(char *out, size_t cap, uint64_t started, uint64_t now) {
  if (started == 0 || started > now) {
    snprintf(out, cap, "-");
    return;
  }
  unsigned long long s = (now - started) / TICKS_PER_SECOND;
  if (s < 60) snprintf(out, cap, "%llus", s);
  else if (s < 3600) snprintf(out, cap, "%llum", s / 60);
  else if (s < 86400) snprintf(out, cap, "%lluh %llum", s / 3600, s % 3600 / 60);
  else snprintf(out, cap, "%llud %lluh", s / 86400, s % 86400 / 3600);
}

void text_short_path(char *out, size_t cap, const char *path, const char *home) {
  if (!home || !home[0] || !path_inside(path, home)) {
    text_copy(out, cap, path);
    return;
  }
  size_t n = strlen(home);
  while (n && home[n - 1] == '\\') n--;
  snprintf(out, cap, "~%s", path + n);
}
