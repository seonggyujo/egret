// Draws into a 32-bit bitmap with Direct2D and turns it into an HICON.
#include "ui/tray_icon.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "base/mem.h"
#include "render/canvas.h"
#include "render/ui_text.h"
#include "ui/icons.h"

#define YELLOW 0xFFF2C230u
#define INK 0xFF1B1B1Bu
#define WHITE 0xFFFFFFFFu

static void draw(Canvas *c, int count, bool light, int size) {
  float s = (float)size;
  if (count <= 0) {
    // The mark's drawing sits left of center in its box; 26 units right centers it.
    icon_mark(c, 26 * s / 256, 0, s, light ? INK : WHITE, YELLOW);
    return;
  }
  canvas_fill_round(c, 0, 0, s, s, s * 0.25f, YELLOW);
  char text[8];
  snprintf(text, sizeof text, "%d", count > 99 ? 99 : count);
  UiFont *f = ui_font_new(L"Bahnschrift", s * (count < 10 ? 0.8f : 0.64f), 600);
  ui_text_draw(c, f, text, 0, s * 0.02f, s, s, INK, ALIGN_CENTER);
  ui_font_free(f);
}

// Direct2D wrote premultiplied colors; icons take straight ones.
static void unpremultiply(uint8_t *bgra, int pixels) {
  for (int i = 0; i < pixels; i++, bgra += 4) {
    unsigned alpha = bgra[3];
    if (alpha == 0 || alpha == 255) continue;
    for (int k = 0; k < 3; k++) bgra[k] = (uint8_t)((bgra[k] * 255u + alpha / 2) / alpha);
  }
}

HICON tray_icon_make(int count, bool light_taskbar, int size) {
  static Canvas *canvas;
  if (!canvas) canvas = canvas_new_bitmap();
  BITMAPINFO bi = {.bmiHeader = {.biSize = sizeof bi.bmiHeader, .biWidth = size, .biHeight = -size,
                                 .biPlanes = 1, .biBitCount = 32, .biCompression = BI_RGB}};
  void *bits = NULL;
  HDC dc = CreateCompatibleDC(NULL);
  HBITMAP color = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
  if (!color) {
    DeleteDC(dc);
    return NULL;
  }
  HGDIOBJ old = SelectObject(dc, color);
  if (canvas_begin(canvas, dc, size, size)) {
    draw(canvas, count, light_taskbar, size);
    canvas_end(canvas);
  }
  GdiFlush();
  unpremultiply(bits, size * size);
  SelectObject(dc, old);
  uint8_t *zeros = xcalloc((size_t)size * (size_t)size / 8 + (size_t)size * 2, 1);
  HBITMAP mask = CreateBitmap(size, size, 1, 1, zeros);
  ICONINFO info = {TRUE, 0, 0, mask, color};
  HICON icon = CreateIconIndirect(&info);
  free(zeros);
  DeleteObject(mask);
  DeleteObject(color);
  DeleteDC(dc);
  return icon;
}
