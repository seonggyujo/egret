// Placing, sizing, showing and hiding the popup.
#include "ui/popup.h"

#include <dwmapi.h>
#include <shellscalingapi.h>

#include "app/theme.h"
#include "ui/hotkey_reg.h"
#include "ui/layout.h"
#include "ui/list_input.h"
#include "ui/scanner.h"
#include "ui/search.h"
#include "ui/settings_input.h"
#include "ui/settings_layout.h"

// Windows 11 attributes the MinGW headers may not name yet.
#define DWMWA_DARK_MODE 20
#define DWMWA_CORNERS 33
#define DWMWA_BORDER 34
#define CORNERS_ROUND 2

#define MARGIN 12  // from the screen's work area edges, in CSS px

static RECT g_work;  // the work area the popup sits in

void popup_init_frame(App *a) {
  DWORD corners = CORNERS_ROUND;
  DwmSetWindowAttribute(a->hwnd, DWMWA_CORNERS, &corners, sizeof corners);
  // A frame margin, however thin, is what keeps the drop shadow.
  MARGINS margins = {0, 0, 1, 0};
  DwmExtendFrameIntoClientArea(a->hwnd, &margins);
  popup_theme_changed(a);
}

void popup_theme_changed(App *a) {
  BOOL dark = theme_dark();
  uint32_t c = theme()->window_border;
  COLORREF border = RGB((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF);
  DwmSetWindowAttribute(a->hwnd, DWMWA_DARK_MODE, &dark, sizeof dark);
  DwmSetWindowAttribute(a->hwnd, DWMWA_BORDER, &border, sizeof border);
  search_theme_changed(a);
  InvalidateRect(a->hwnd, NULL, FALSE);
}

static int content_height(const App *a) {
  if (a->view == POPUP_SETTINGS) return settings_height(a);
  int body = a->visible_count == 0 ? a->m.empty_h : a->m.colhead_h + layout_rows_shown(a) * a->m.row_h;
  return a->m.header_h + body + a->m.status_h;
}

void popup_fit(App *a) {
  int w = a->m.width, h = content_height(a), margin = app_px(a, MARGIN);
  int x = g_work.right - margin - w, y = g_work.bottom - margin - h;
  SetWindowPos(a->hwnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE);
  InvalidateRect(a->hwnd, NULL, FALSE);
}

// The monitor under the mouse: its work area, and its DPI for the sizes.
static void take_mouse_monitor(App *a) {
  POINT pt;
  GetCursorPos(&pt);
  HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
  MONITORINFO mi = {.cbSize = sizeof mi};
  GetMonitorInfoW(mon, &mi);
  g_work = mi.rcWork;
  UINT dx = 96, dy = 96;
  if (SUCCEEDED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dx, &dy)) && dx != a->dpi) {
    app_set_dpi(a, dx);
    search_place(a);
  }
}

void popup_set_view(App *a, View view) {
  if (view == POPUP_SETTINGS) settings_open(a);
  a->view = view;
  a->confirm.active = false;
  a->hover = -1;
  a->hover_part = 0;
  search_place(a);
  popup_fit(a);
  SetFocus(view == POPUP_LIST ? a->search : a->hwnd);
}

void popup_show(App *a, View view) {
  take_mouse_monitor(a);
  search_clear(a);
  a->top = 0;
  a->selected = a->visible_count ? 0 : -1;
  scanner_wake();
  a->shown = true;
  popup_set_view(a, view);
  // Not ShowWindow: the launcher's STARTUPINFO can override its first call.
  SetWindowPos(a->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
  SetForegroundWindow(a->hwnd);
  SetFocus(view == POPUP_LIST ? a->search : a->hwnd);
  SetTimer(a->hwnd, TIMER_TICK, 1000, NULL);
}

void popup_hide(App *a) {
  if (a->capturing_hotkey) {
    a->capturing_hotkey = false;
    hotkey_reg_resume(a);
  }
  a->shown = false;
  a->confirm.active = false;
  KillTimer(a->hwnd, TIMER_TICK);
  SetWindowPos(a->hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);
}

void popup_toggle(App *a) {
  if (a->shown) popup_hide(a);
  else popup_show(a, POPUP_LIST);
}

void popup_activated(App *a, bool active) {
  if (active) {
    SetFocus(a->view == POPUP_LIST ? a->search : a->hwnd);
  } else if (a->shown) {
    popup_hide(a);
    a->hidden_at = GetTickCount64();
  }
}

void popup_dpi_changed(App *a, UINT dpi) {
  if (dpi == a->dpi) return;
  app_set_dpi(a, dpi);
  search_place(a);
  popup_fit(a);
}

// The status bar moves with the bottom edge, so all of it repaints.
void popup_resized(App *a) { InvalidateRect(a->hwnd, NULL, FALSE); }

void popup_tick(App *a) {
  ULONGLONG now = GetTickCount64();
  if (a->confirm.active && now > a->confirm.until && !list_mouse_on_confirm(a)) a->confirm.active = false;
  if (a->note[0] && now > a->note_until) a->note[0] = 0;
  InvalidateRect(a->hwnd, NULL, FALSE);
}
