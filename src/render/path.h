// Vector shapes written as numbers, so icons stay SVG-sharp at any DPI: an
// op code, then its coordinates in the shape's own design units.
//   PATH_MOVE x y            starts a figure
//   PATH_LINE x y
//   PATH_CUBIC x1 y1 x2 y2 x y
//   PATH_ARC x y r cw large  a circular arc to (x, y), as SVG's "a r r 0 large cw x y"
//   PATH_CLOSE               closes the figure
#pragma once

#include <stdint.h>

#include "render/canvas.h"

enum { PATH_MOVE, PATH_LINE, PATH_CUBIC, PATH_ARC, PATH_CLOSE };

typedef struct {
  const float *ops;
  int count;
} Path;

#define PATH(array) ((Path){array, (int)(sizeof(array) / sizeof(array[0]))})

// Draws p with its design units scaled by scale and its origin at (x, y).
void canvas_fill_path(Canvas *c, Path p, float x, float y, float scale, uint32_t argb);
// Round caps and joins; width is in pixels.
void canvas_stroke_path(Canvas *c, Path p, float x, float y, float scale, float width, uint32_t argb);
