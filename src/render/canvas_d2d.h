// The Direct2D objects behind a Canvas, for the drawing code in render/.
// Nothing outside render/ includes this.
#pragma once

#include <d2d1.h>

#include "render/canvas.h"

ID2D1RenderTarget *canvas_target(Canvas *c);
ID2D1Factory *canvas_factory(Canvas *c);

// The canvas's one brush, set to argb. Valid until the next call.
ID2D1Brush *canvas_brush(Canvas *c, uint32_t argb);
// The round-capped, round-joined stroke style icons use.
ID2D1StrokeStyle *canvas_round_style(Canvas *c);
