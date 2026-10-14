// Integer pixel rectangles and whether a point is inside one.
#pragma once

#include <stdbool.h>

typedef struct {
  int x, y, w, h;
} Box;

static inline bool box_contains(Box b, int x, int y) {
  return x >= b.x && y >= b.y && x < b.x + b.w && y < b.y + b.h;
}
