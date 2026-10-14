// The app's state: the popup window and its drawing objects at the current
// DPI, the rows from the last scan and what the list shows of them, the
// settings view, the tray icon and the global shortcut. Only ui/ includes
// this; the window procedure owns the one App.
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <windows.h>
// After windows.h, which defines what shellapi.h uses.
#include <shellapi.h>

#include "app/hidden.h"
#include "base/box.h"
#include "model/listener.h"
#include "render/canvas.h"
#include "render/ui_text.h"

// Messages the app sends itself.
#define WM_APP_SCANNED (WM_APP + 1)  // lParam: a ListenerList* the window now owns
#define WM_APP_STOPPED (WM_APP + 2)  // lParam: the StopJob* that finished
#define WM_APP_SHOW (WM_APP + 3)     // a second launch asks for the popup
#define WM_APP_TRAY (WM_APP + 4)     // the tray icon's callback

#define TIMER_TICK 1  // once a second while the popup shows

typedef enum { POPUP_LIST, POPUP_SETTINGS } View;

// Sizes in physical pixels at the current DPI.
typedef struct {
  int width, header_h, colhead_h, row_h, status_h, empty_h, pad_x, gap;
  int col_port, col_name, col_tags, col_up, button_w, button_h;
} Metrics;

// A row waiting for the second press (the yellow row), or being stopped.
typedef struct {
  bool active, admin;
  uint16_t port;
  uint32_t pid;
  ULONGLONG until;  // the confirm lapses then (GetTickCount64)
} Pending;

typedef struct App {
  HINSTANCE instance;
  HWND hwnd, search, tooltip;
  HFONT search_font;
  HBRUSH search_brush;
  Canvas *canvas;
  UINT dpi;
  float scale;
  Metrics m;
  UiFont *font, *font_small, *font_tag, *font_title, *font_port, *font_num;
  View view;
  bool shown;
  ULONGLONG hidden_at;  // when the popup last hid because it lost focus

  // The list.
  ListenerList rows;
  int *visible;  // indexes into rows, in the order shown
  int visible_count, visible_cap;
  HiddenList hidden;
  bool show_all;
  char query[256];
  int selected, hover, hover_part, top;  // visible indexes (-1 none), first row shown
  Pending confirm, stopping;
  char note[256];  // the last stop's outcome, shown in the status bar
  bool note_error;
  ULONGLONG note_until;

  // The settings view.
  int settings_hover;
  bool capturing_hotkey, autostart, user_path;

  // The tray icon and the shortcut.
  NOTIFYICONDATAW tray;
  HICON tray_icon;
  int tray_count;
  bool tray_light;
  UINT taskbar_created;
  unsigned hotkey_mods, hotkey_vk;
  bool hotkey_ok;

  // The tooltip's current target.
  Box tip_box;
  wchar_t tip_text[512];
} App;

// A CSS-like size in physical pixels.
int app_px(const App *a, float css);
// Scale, metrics and fonts for a DPI.
void app_set_dpi(App *a, UINT dpi);
void app_free_fonts(App *a);
// The row at a visible index, NULL when out of range.
const Listener *app_row(const App *a, int visible_index);
// Shows text in the status bar for a few seconds.
void app_note(App *a, const char *text, bool error);
