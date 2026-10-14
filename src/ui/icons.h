// The popup's line icons and the egret mark, drawn from vector paths so they
// stay sharp at every DPI. No font glyphs or emoji.
#pragma once

#include <stdint.h>

#include "base/box.h"
#include "render/canvas.h"

typedef enum { ICON_GEAR, ICON_BROWSER, ICON_FOLDER, ICON_POWER, ICON_LOCK, ICON_BACK, ICON_CLOSE } IconId;

// A 16-unit line icon, size pixels wide, centered in b, strokes 1.4 units.
void icon_draw(Canvas *c, IconId id, Box b, float size, uint32_t color);

// The egret mark (the head, neck and bill from res/egret.svg) in a size by
// size square at (x, y). Below 24 px it uses the simplified small drawing.
void icon_mark(Canvas *c, float x, float y, float size, uint32_t body, uint32_t bill);
