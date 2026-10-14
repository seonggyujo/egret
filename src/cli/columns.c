// Column widths of UTF-8 text for lining up the --list table.
#include "cli/columns.h"

#include <stdbool.h>
#include <stdint.h>

static bool wide(uint32_t cp) {
  return (cp >= 0x1100 && cp <= 0x115F) || (cp >= 0x2E80 && cp <= 0xA4CF) || (cp >= 0xAC00 && cp <= 0xD7A3) ||
         (cp >= 0xF900 && cp <= 0xFAFF) || (cp >= 0xFE30 && cp <= 0xFE4F) || (cp >= 0xFF00 && cp <= 0xFF60) ||
         (cp >= 0xFFE0 && cp <= 0xFFE6) || (cp >= 0x20000 && cp <= 0x3FFFD);
}

int text_columns(const char *utf8) {
  int cols = 0;
  for (const unsigned char *p = (const unsigned char *)utf8; *p;) {
    uint32_t cp = *p;
    int extra = cp >= 0xF0 ? 3 : cp >= 0xE0 ? 2 : cp >= 0xC0 ? 1 : 0;
    if (extra) cp &= 0x3F >> extra;
    p++;
    for (int i = 0; i < extra && (*p & 0xC0) == 0x80; i++) cp = (cp << 6) | (*p++ & 0x3F);
    cols += wide(cp) ? 2 : 1;
  }
  return cols;
}
