// The window class, setting up the app, and the message switch.
#include "ui/window.h"

#include <wchar.h>
#include <windowsx.h>

#include "app/hidden.h"
#include "app/prefs.h"
#include "app/theme.h"
#include "base/log.h"
#include "ui/app.h"
#include "ui/hotkey_reg.h"
#include "ui/keys.h"
#include "ui/mouse.h"
#include "ui/paint.h"
#include "ui/popup.h"
#include "ui/rows_view.h"
#include "ui/scanner.h"
#include "ui/search.h"
#include "ui/single.h"
#include "ui/stopper.h"
#include "ui/tooltip.h"
#include "ui/tray.h"

static App g_app;

static void on_create(App *a, HWND hwnd) {
  a->hwnd = hwnd;
  a->selected = a->hover = -1;
  app_set_dpi(a, GetDpiForWindow(hwnd));
  a->canvas = canvas_new(hwnd);
  popup_init_frame(a);
  search_create(a);
  tooltip_create(a);
  hidden_load(&a->hidden);
  a->show_all = prefs_get_bool(PREF_SHOW_ALL, false);
  a->taskbar_created = RegisterWindowMessageW(L"TaskbarCreated");
  tray_add(a);
  hotkey_reg_load(a);
  scanner_start(hwnd);
}

static void on_destroy(App *a) {
  scanner_stop();
  tray_remove(a);
  canvas_free(a->canvas);
  a->canvas = NULL;
  app_free_fonts(a);
  PostQuitMessage(0);
}

static void on_setting_change(App *a, LPARAM lp) {
  if (!lp || wcscmp((const wchar_t *)lp, L"ImmersiveColorSet") != 0) return;
  theme_system_changed();
  popup_theme_changed(a);
  tray_refresh(a);
}

static LRESULT mouse_message(App *a, UINT msg, WPARAM wp, LPARAM lp) {
  int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
  switch (msg) {
    case WM_MOUSEMOVE: mouse_move(a, x, y); break;
    case WM_MOUSELEAVE: mouse_leave(a); break;
    case WM_LBUTTONUP: mouse_click(a, x, y); break;
    case WM_LBUTTONDBLCLK: mouse_double_click(a, x, y); break;
    case WM_RBUTTONUP: mouse_right_click(a, x, y); break;
    case WM_MOUSEWHEEL: mouse_wheel(a, GET_WHEEL_DELTA_WPARAM(wp)); break;
    default: break;
  }
  return 0;
}

static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  App *a = &g_app;
  if (a->taskbar_created && msg == a->taskbar_created) {
    tray_add(a);  // Explorer restarted
    return 0;
  }
  switch (msg) {
    case WM_CREATE: on_create(a, hwnd); return 0;
    case WM_NCCALCSIZE: if (wp) return 0; break;  // the whole window is client area
    case WM_NCHITTEST: return HTCLIENT;           // no resizing edges
    case WM_DPICHANGED: popup_dpi_changed(a, HIWORD(wp)); return 0;
    case WM_SIZE: popup_resized(a); return 0;
    case WM_PAINT: paint(a); return 0;
    case WM_ERASEBKGND: return 1;
    case WM_ACTIVATE: popup_activated(a, LOWORD(wp) != WA_INACTIVE); return 0;
    case WM_CLOSE: popup_hide(a); return 0;
    case WM_TIMER: popup_tick(a); return 0;
    case WM_HOTKEY: popup_toggle(a); return 0;
    case WM_APP_SHOW: popup_show(a, POPUP_LIST); return 0;
    case WM_APP_SCANNED: rows_view_take(a, (ListenerList *)lp); return 0;
    case WM_APP_STOPPED: stopper_done(a, (StopJob *)lp); return 0;
    case WM_APP_TRAY: tray_event(a, wp, lp); return 0;
    case WM_COMMAND:
      if ((HWND)lp == a->search && HIWORD(wp) == EN_CHANGE) search_changed(a);
      return 0;
    case WM_CTLCOLOREDIT: return search_colors(a, (HDC)wp);
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
      if (keys_popup(a, (UINT)wp)) return 0;
      break;
    case WM_MOUSEMOVE:
    case WM_MOUSELEAVE:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONUP:
    case WM_MOUSEWHEEL: return mouse_message(a, msg, wp, lp);
    case WM_SETTINGCHANGE: on_setting_change(a, lp); break;
    case WM_NOTIFY:
      if (tooltip_notify(a, lp)) return 0;
      break;
    case WM_DESTROY: on_destroy(a); return 0;
    default: break;
  }
  return DefWindowProcW(hwnd, msg, wp, lp);
}

bool window_create(HINSTANCE instance, bool show_popup) {
  WNDCLASSEXW wc = {.cbSize = sizeof wc};
  wc.style = CS_DBLCLKS;
  wc.lpfnWndProc = window_proc;
  wc.hInstance = instance;
  wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
  wc.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(1));
  wc.lpszClassName = SINGLE_CLASS;
  if (!RegisterClassExW(&wc)) return false;
  g_app.instance = instance;
  // A thick frame keeps the system shadow; WM_NCCALCSIZE hides the frame.
  HWND hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, SINGLE_CLASS, L"egret",
                              WS_POPUP | WS_THICKFRAME | WS_CLIPCHILDREN, 0, 0, 100, 100, NULL, NULL,
                              instance, NULL);
  if (!hwnd) {
    log_error("window", "CreateWindowEx failed: %lu", GetLastError());
    return false;
  }
  if (show_popup) popup_show(&g_app, POPUP_LIST);
  return true;
}
