// Drawing with Direct2D into a GDI device context: the window's paint DC
// (so child windows such as the search box stay on top) or a 32-bit bitmap
// (the tray icon). Each frame is drawn off screen and copied in one go.
// Coordinates are physical pixels (targets run at 96 DPI; the UI scales its
// own sizes). Colors are 0xAARRGGBB. Shapes are anti-aliased.
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

typedef struct Canvas Canvas;

// For a window; hwnd is repainted when the device is lost.
Canvas *canvas_new(HWND hwnd);
// For a transparent bitmap: its frames start cleared, colors premultiplied.
Canvas *canvas_new_bitmap(void);
void canvas_free(Canvas *c);

// Every frame is begin, draw, end. begin draws into dc's top-left width by
// height; false when there is nothing to draw on.
bool canvas_begin(Canvas *c, HDC dc, int width, int height);
void canvas_end(Canvas *c);

void canvas_clear(Canvas *c, uint32_t argb);
void canvas_fill_rect(Canvas *c, float x, float y, float w, float h, uint32_t argb);
void canvas_fill_round(Canvas *c, float x, float y, float w, float h, float radius, uint32_t argb);
// The stroke stays inside the box.
void canvas_stroke_round(Canvas *c, float x, float y, float w, float h, float radius, float width,
                         uint32_t argb);
void canvas_fill_circle(Canvas *c, float cx, float cy, float radius, uint32_t argb);

// Clips drawing to a box until the matching pop.
void canvas_push_clip(Canvas *c, float x, float y, float w, float h);
void canvas_pop_clip(Canvas *c);
