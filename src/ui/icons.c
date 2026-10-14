// Path data for the icons (a 16-unit box, from the design mockup's SVGs)
// and the mark (a 256-unit box, from res/egret.svg and res/egret-16.svg).
#include "ui/icons.h"

#include "render/path.h"

#define M PATH_MOVE
#define L PATH_LINE
#define C PATH_CUBIC
#define A PATH_ARC
#define Z PATH_CLOSE

static const float GEAR[] = {
    M, 10.3f, 8, A, 5.7f, 8, 2.3f, 1, 0, A, 10.3f, 8, 2.3f, 1, 0,
    M, 8, 1.8f, L, 8, 3.5f, M, 8, 12.5f, L, 8, 14.2f, M, 1.8f, 8, L, 3.5f, 8, M, 12.5f, 8, L, 14.2f, 8,
    M, 3.6f, 3.6f, L, 4.8f, 4.8f, M, 11.2f, 11.2f, L, 12.4f, 12.4f,
    M, 3.6f, 12.4f, L, 4.8f, 11.2f, M, 11.2f, 4.8f, L, 12.4f, 3.6f};
static const float BROWSER[] = {
    M, 9.5f, 2.5f, L, 13.5f, 2.5f, L, 13.5f, 6.5f, M, 13.5f, 2.5f, L, 8, 8,
    M, 11.5f, 9.5f, L, 11.5f, 12.5f, A, 10.5f, 13.5f, 1, 1, 0, L, 3.5f, 13.5f, A, 2.5f, 12.5f, 1, 1, 0,
    L, 2.5f, 5.5f, A, 3.5f, 4.5f, 1, 1, 0, L, 6.5f, 4.5f};
static const float FOLDER[] = {
    M, 2.5f, 4.5f, A, 3.5f, 3.5f, 1, 1, 0, L, 6.5f, 3.5f, L, 8, 5, L, 12.5f, 5, A, 13.5f, 6, 1, 1, 0,
    L, 13.5f, 11.5f, A, 12.5f, 12.5f, 1, 1, 0, L, 3.5f, 12.5f, A, 2.5f, 11.5f, 1, 1, 0, Z};
static const float POWER[] = {M, 8, 2.2f, L, 8, 7.5f, M, 4.6f, 4.4f, A, 11.4f, 4.4f, 4.9f, 0, 1};
static const float LOCK[] = {
    M, 4.7f, 7, L, 11.3f, 7, A, 12.5f, 8.2f, 1.2f, 1, 0, L, 12.5f, 12.3f, A, 11.3f, 13.5f, 1.2f, 1, 0,
    L, 4.7f, 13.5f, A, 3.5f, 12.3f, 1.2f, 1, 0, L, 3.5f, 8.2f, A, 4.7f, 7, 1.2f, 1, 0, Z,
    M, 5.5f, 7, L, 5.5f, 5.2f, A, 10.5f, 5.2f, 2.5f, 1, 0, L, 10.5f, 7};
static const float BACK[] = {M, 10, 3.5f, L, 5.5f, 8, L, 10, 12.5f};
static const float CLOSE[] = {M, 4, 4, L, 12, 12, M, 12, 4, L, 4, 12};

static const float MARK[] = {
    M, 89, 58, C, 106, 38, 124, 31, 141, 34, C, 160, 37, 177, 48, 192, 63, L, 176, 59,
    C, 187, 72, 191, 86, 187, 101, C, 182, 120, 165, 134, 148, 147, C, 132, 160, 127, 174, 130, 187,
    C, 132, 200, 144, 213, 160, 224, C, 140, 221, 121, 211, 111, 196, C, 100, 179, 102, 158, 112, 143,
    C, 122, 127, 139, 115, 151, 103, C, 160, 94, 162, 84, 156, 77, C, 149, 67, 135, 63, 120, 64,
    C, 109, 65, 100, 71, 91, 78, Z};
static const float MARK_BILL[] = {M, 14, 98, L, 89, 58, L, 117, 79, Z};
static const float MARK_SMALL[] = {
    M, 85, 65, C, 101, 43, 121, 35, 142, 39, C, 158, 42, 174, 53, 187, 67, L, 169, 62,
    C, 181, 77, 184, 91, 178, 105, C, 171, 122, 154, 135, 136, 150, C, 121, 163, 118, 178, 126, 193,
    C, 133, 207, 148, 218, 163, 225, C, 140, 222, 120, 210, 109, 192, C, 99, 176, 101, 157, 112, 140,
    C, 124, 123, 141, 111, 151, 99, C, 159, 90, 159, 81, 152, 74, C, 142, 64, 122, 64, 107, 73, Z};
static const float MARK_SMALL_BILL[] = {M, 12, 98, L, 86, 64, L, 119, 80, Z};

static Path icon_path(IconId id) {
  switch (id) {
    case ICON_GEAR: return PATH(GEAR);
    case ICON_BROWSER: return PATH(BROWSER);
    case ICON_FOLDER: return PATH(FOLDER);
    case ICON_POWER: return PATH(POWER);
    case ICON_LOCK: return PATH(LOCK);
    case ICON_BACK: return PATH(BACK);
    default: return PATH(CLOSE);
  }
}

void icon_draw(Canvas *c, IconId id, Box b, float size, uint32_t color) {
  float s = size / 16.0f;
  float x = b.x + (b.w - size) / 2.0f, y = b.y + (b.h - size) / 2.0f;
  canvas_stroke_path(c, icon_path(id), x, y, s, 1.4f * s, color);
}

void icon_mark(Canvas *c, float x, float y, float size, uint32_t body, uint32_t bill) {
  float s = size / 256.0f;
  bool small = size < 24;
  canvas_fill_path(c, small ? PATH(MARK_SMALL) : PATH(MARK), x, y, s, body);
  canvas_fill_path(c, small ? PATH(MARK_SMALL_BILL) : PATH(MARK_BILL), x, y, s, bill);
}
