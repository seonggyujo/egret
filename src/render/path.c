// Building Direct2D path geometries from the numbers in path.h.
#include "render/path.h"

#include "render/canvas_d2d.h"

static D2D1_POINT_2F point(const float *v, float x, float y, float s) {
  D2D1_POINT_2F p = {x + v[0] * s, y + v[1] * s};
  return p;
}

// Feeds the ops to the sink. Figures left open stay open (strokes).
static void feed(ID2D1GeometrySink *sink, Path p, float x, float y, float s, bool filled) {
  bool open = false;
  D2D1_FIGURE_BEGIN begin = filled ? D2D1_FIGURE_BEGIN_FILLED : D2D1_FIGURE_BEGIN_HOLLOW;
  for (int i = 0; i < p.count;) {
    int op = (int)p.ops[i++];
    const float *v = &p.ops[i];
    if (op == PATH_MOVE) {
      if (open) ID2D1GeometrySink_EndFigure(sink, D2D1_FIGURE_END_OPEN);
      ID2D1GeometrySink_BeginFigure(sink, point(v, x, y, s), begin);
      open = true;
      i += 2;
    } else if (op == PATH_LINE) {
      ID2D1GeometrySink_AddLine(sink, point(v, x, y, s));
      i += 2;
    } else if (op == PATH_CUBIC) {
      D2D1_BEZIER_SEGMENT b = {point(v, x, y, s), point(v + 2, x, y, s), point(v + 4, x, y, s)};
      ID2D1GeometrySink_AddBezier(sink, &b);
      i += 6;
    } else if (op == PATH_ARC) {
      D2D1_ARC_SEGMENT a = {point(v, x, y, s), {v[2] * s, v[2] * s}, 0,
                            v[3] != 0 ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
                            v[4] != 0 ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL};
      ID2D1GeometrySink_AddArc(sink, &a);
      i += 5;
    } else {
      if (open) ID2D1GeometrySink_EndFigure(sink, D2D1_FIGURE_END_CLOSED);
      open = false;
    }
  }
  if (open) ID2D1GeometrySink_EndFigure(sink, D2D1_FIGURE_END_OPEN);
}

static ID2D1PathGeometry *build(Canvas *c, Path p, float x, float y, float s, bool filled) {
  ID2D1PathGeometry *g = NULL;
  ID2D1GeometrySink *sink = NULL;
  if (FAILED(ID2D1Factory_CreatePathGeometry(canvas_factory(c), &g))) return NULL;
  if (FAILED(ID2D1PathGeometry_Open(g, &sink))) {
    ID2D1PathGeometry_Release(g);
    return NULL;
  }
  feed(sink, p, x, y, s, filled);
  ID2D1GeometrySink_Close(sink);
  ID2D1GeometrySink_Release(sink);
  return g;
}

void canvas_fill_path(Canvas *c, Path p, float x, float y, float scale, uint32_t argb) {
  ID2D1PathGeometry *g = build(c, p, x, y, scale, true);
  if (!g) return;
  ID2D1RenderTarget_FillGeometry(canvas_target(c), (ID2D1Geometry *)g, canvas_brush(c, argb), NULL);
  ID2D1PathGeometry_Release(g);
}

void canvas_stroke_path(Canvas *c, Path p, float x, float y, float scale, float width, uint32_t argb) {
  ID2D1PathGeometry *g = build(c, p, x, y, scale, false);
  if (!g) return;
  ID2D1RenderTarget_DrawGeometry(canvas_target(c), (ID2D1Geometry *)g, canvas_brush(c, argb), width,
                                 canvas_round_style(c));
  ID2D1PathGeometry_Release(g);
}
