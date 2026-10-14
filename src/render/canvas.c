// A Direct2D DC render target and the objects shared by everything drawn
// on it.
#include <initguid.h>

#include "render/canvas_d2d.h"

#include <stdlib.h>

#include "base/log.h"
#include "base/mem.h"
#include "render/fonts.h"

struct Canvas {
  HWND hwnd;         // NULL for a bitmap canvas
  bool transparent;  // a bitmap: premultiplied alpha, cleared each frame
  ID2D1Factory *factory;
  ID2D1DCRenderTarget *dc_target;
  ID2D1RenderTarget *target;  // the same object, as its base interface
  ID2D1SolidColorBrush *brush;
  ID2D1StrokeStyle *round;  // made on first use
};

static void release_target(Canvas *c) {
  if (c->brush) ID2D1SolidColorBrush_Release(c->brush);
  if (c->dc_target) ID2D1DCRenderTarget_Release(c->dc_target);
  c->brush = NULL;
  c->dc_target = NULL;
  c->target = NULL;
}

// Grayscale text works on transparent bitmaps too; without contrast
// enhancement text keeps the weight it was drawn with.
static void setup_text(Canvas *c) {
  ID2D1RenderTarget_SetTextAntialiasMode(c->target, D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
  IDWriteRenderingParams *params = NULL;
  if (fonts_factory() &&
      SUCCEEDED(IDWriteFactory_CreateCustomRenderingParams(fonts_factory(), 1.8f, 0.0f, 0.0f,
                                                           DWRITE_PIXEL_GEOMETRY_FLAT,
                                                           DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC, &params))) {
    ID2D1RenderTarget_SetTextRenderingParams(c->target, params);
    IDWriteRenderingParams_Release(params);
  }
}

static bool create_target(Canvas *c) {
  D2D1_ALPHA_MODE alpha = c->transparent ? D2D1_ALPHA_MODE_PREMULTIPLIED : D2D1_ALPHA_MODE_IGNORE;
  D2D1_RENDER_TARGET_PROPERTIES props = {
      D2D1_RENDER_TARGET_TYPE_DEFAULT, {DXGI_FORMAT_B8G8R8A8_UNORM, alpha}, 96.0f, 96.0f,
      D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT};
  HRESULT hr = ID2D1Factory_CreateDCRenderTarget(c->factory, &props, &c->dc_target);
  if (FAILED(hr)) {
    log_error("canvas", "CreateDCRenderTarget failed: 0x%08lx", (unsigned long)hr);
    return false;
  }
  c->target = (ID2D1RenderTarget *)c->dc_target;
  D2D1_COLOR_F white = {1, 1, 1, 1};
  ID2D1RenderTarget_CreateSolidColorBrush(c->target, &white, NULL, &c->brush);
  setup_text(c);
  return true;
}

static Canvas *canvas_alloc(HWND hwnd, bool transparent) {
  Canvas *c = xcalloc(1, sizeof *c);
  c->hwnd = hwnd;
  c->transparent = transparent;
  D2D1_FACTORY_OPTIONS options = {D2D1_DEBUG_LEVEL_NONE};
  HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &IID_ID2D1Factory, &options,
                                 (void **)&c->factory);
  if (FAILED(hr)) log_error("canvas", "D2D1CreateFactory failed: 0x%08lx", (unsigned long)hr);
  return c;
}

Canvas *canvas_new(HWND hwnd) { return canvas_alloc(hwnd, false); }

Canvas *canvas_new_bitmap(void) { return canvas_alloc(NULL, true); }

void canvas_free(Canvas *c) {
  if (!c) return;
  release_target(c);
  if (c->round) ID2D1StrokeStyle_Release(c->round);
  if (c->factory) ID2D1Factory_Release(c->factory);
  free(c);
}

bool canvas_begin(Canvas *c, HDC dc, int width, int height) {
  if (!c->factory) return false;
  if (!c->target && !create_target(c)) return false;
  RECT rc = {0, 0, width, height};
  if (FAILED(ID2D1DCRenderTarget_BindDC(c->dc_target, dc, &rc))) return false;
  ID2D1RenderTarget_BeginDraw(c->target);
  if (c->transparent) {
    D2D1_COLOR_F clear = {0, 0, 0, 0};
    ID2D1RenderTarget_Clear(c->target, &clear);
  }
  return true;
}

void canvas_end(Canvas *c) {
  HRESULT hr = ID2D1RenderTarget_EndDraw(c->target, NULL, NULL);
  // The GPU was reset or changed: start over with a new target next frame.
  if (hr == (HRESULT)D2DERR_RECREATE_TARGET) {
    log_warn("canvas", "render target lost, recreating");
    release_target(c);
    if (c->hwnd) InvalidateRect(c->hwnd, NULL, FALSE);
  }
}

ID2D1RenderTarget *canvas_target(Canvas *c) { return c->target; }

ID2D1Factory *canvas_factory(Canvas *c) { return c->factory; }

ID2D1Brush *canvas_brush(Canvas *c, uint32_t argb) {
  D2D1_COLOR_F color = {((argb >> 16) & 0xFF) / 255.0f, ((argb >> 8) & 0xFF) / 255.0f,
                        (argb & 0xFF) / 255.0f, ((argb >> 24) & 0xFF) / 255.0f};
  ID2D1SolidColorBrush_SetColor(c->brush, &color);
  return (ID2D1Brush *)c->brush;
}

ID2D1StrokeStyle *canvas_round_style(Canvas *c) {
  if (!c->round) {
    D2D1_STROKE_STYLE_PROPERTIES props = {D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
                                          D2D1_LINE_JOIN_ROUND, 10.0f, D2D1_DASH_STYLE_SOLID, 0.0f};
    ID2D1Factory_CreateStrokeStyle(c->factory, &props, NULL, 0, &c->round);
  }
  return c->round;
}
