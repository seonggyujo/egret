// UI text: one line, vertically centered in its box, cut with an ellipsis
// when it does not fit. Strings are UTF-8.
#pragma once

#include "render/canvas.h"

typedef struct UiFont UiFont;
typedef enum { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT } TextAlign;

// px is the font size in pixels, already scaled for the screen; weight as
// in CSS (400 normal, 600 semibold).
UiFont *ui_font_new(const wchar_t *family, float px, int weight);
void ui_font_free(UiFont *f);

float ui_text_width(UiFont *f, const char *utf8);
void ui_text_draw(Canvas *c, UiFont *f, const char *utf8, float x, float y, float w, float h,
                  uint32_t argb, TextAlign align);
