// Shell_NotifyIcon calls, the tray's clicks and its menu.
#include "ui/tray.h"

#include <stdio.h>
#include <wchar.h>
#include <windowsx.h>

#include "app/strings.h"
#include "app/theme.h"
#include "ui/hotkey_reg.h"
#include "ui/popup.h"
#include "ui/rows_view.h"
#include "ui/tray_icon.h"

#define TRAY_ID 1
#define REOPEN_MS 400  // a click that just hid the popup must not reopen it

enum { MENU_OPEN = 1, MENU_SHOW_ALL, MENU_SETTINGS, MENU_QUIT };

static void set_tip(App *a) {
  char tip[128];
  format_tray_tip(tip, sizeof tip, a->tray_count);
  MultiByteToWideChar(CP_UTF8, 0, tip, -1, a->tray.szTip, ARRAYSIZE(a->tray.szTip));
}

static void make_icon(App *a) {
  if (a->tray_icon) DestroyIcon(a->tray_icon);
  a->tray_light = theme_taskbar_light();
  int size = GetSystemMetricsForDpi(SM_CXSMICON, GetDpiForSystem());
  a->tray_icon = tray_icon_make(a->tray_count, a->tray_light, size);
  a->tray.hIcon = a->tray_icon;
}

void tray_add(App *a) {
  NOTIFYICONDATAW *n = &a->tray;
  n->cbSize = sizeof *n;
  n->hWnd = a->hwnd;
  n->uID = TRAY_ID;
  n->uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;
  n->uCallbackMessage = WM_APP_TRAY;
  make_icon(a);
  set_tip(a);
  Shell_NotifyIconW(NIM_ADD, n);
  n->uVersion = NOTIFYICON_VERSION_4;
  Shell_NotifyIconW(NIM_SETVERSION, n);
}

void tray_remove(App *a) {
  a->tray.uFlags = 0;
  Shell_NotifyIconW(NIM_DELETE, &a->tray);
  if (a->tray_icon) DestroyIcon(a->tray_icon);
  a->tray_icon = NULL;
}

static void modify(App *a) {
  a->tray.uFlags = NIF_ICON | NIF_TIP | NIF_SHOWTIP;
  Shell_NotifyIconW(NIM_MODIFY, &a->tray);
}

void tray_refresh(App *a) {
  make_icon(a);
  set_tip(a);
  modify(a);
}

void tray_set_count(App *a, int count) {
  if (count == a->tray_count && a->tray_light == theme_taskbar_light()) return;
  a->tray_count = count;
  tray_refresh(a);
}

void tray_notify(App *a, const char *text) {
  NOTIFYICONDATAW n = a->tray;
  n.uFlags = NIF_INFO;
  n.dwInfoFlags = NIIF_NONE | NIIF_RESPECT_QUIET_TIME;
  wcscpy(n.szInfoTitle, L"egret");
  MultiByteToWideChar(CP_UTF8, 0, text, -1, n.szInfo, ARRAYSIZE(n.szInfo));
  Shell_NotifyIconW(NIM_MODIFY, &n);
}

static void append(HMENU menu, UINT id, const char *text, bool checked) {
  wchar_t w[128];
  MultiByteToWideChar(CP_UTF8, 0, text, -1, w, 128);
  AppendMenuW(menu, MF_STRING | (checked ? MF_CHECKED : 0), id, w);
}

static void show_menu(App *a, int x, int y) {
  const Strings *s = strings();
  char open[128], key[64];
  hotkey_reg_text(a, key, sizeof key);
  snprintf(open, sizeof open, "%s\t%s", s->tray_open, key);
  HMENU menu = CreatePopupMenu();
  append(menu, MENU_OPEN, open, false);
  append(menu, MENU_SHOW_ALL, s->tray_show_all, a->show_all);
  AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
  append(menu, MENU_SETTINGS, s->tray_settings, false);
  append(menu, MENU_QUIT, s->tray_quit, false);
  SetMenuDefaultItem(menu, MENU_OPEN, FALSE);
  SetForegroundWindow(a->hwnd);  // so the menu closes when clicked away from
  UINT align = GetSystemMetrics(SM_MENUDROPALIGNMENT) ? TPM_RIGHTALIGN : TPM_LEFTALIGN;
  int cmd = TrackPopupMenuEx(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | align, x, y, a->hwnd, NULL);
  PostMessageW(a->hwnd, WM_NULL, 0, 0);
  DestroyMenu(menu);
  if (cmd == MENU_OPEN) popup_show(a, POPUP_LIST);
  if (cmd == MENU_SHOW_ALL) rows_view_toggle_all(a);
  if (cmd == MENU_SETTINGS) popup_show(a, POPUP_SETTINGS);
  if (cmd == MENU_QUIT) DestroyWindow(a->hwnd);
}

void tray_event(App *a, WPARAM wp, LPARAM lp) {
  switch (LOWORD(lp)) {
    case NIN_SELECT:
    case NIN_KEYSELECT:
      if (!a->shown && GetTickCount64() - a->hidden_at < REOPEN_MS) break;
      popup_toggle(a);
      break;
    case WM_CONTEXTMENU: show_menu(a, GET_X_LPARAM(wp), GET_Y_LPARAM(wp)); break;
    default: break;
  }
}
