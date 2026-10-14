// The search edit control and the keys it hands to the list.
#include "ui/search.h"

#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>

#include "app/strings.h"
#include "app/theme.h"
#include "base/utf.h"
#include "ui/keys.h"
#include "ui/layout.h"
#include "ui/popup.h"
#include "ui/rows_view.h"

#define SUBCLASS_ID 1

static COLORREF rgb(uint32_t argb) { return RGB((argb >> 16) & 0xFF, (argb >> 8) & 0xFF, argb & 0xFF); }

// Whether Delete has nothing left to delete: no selection, caret at the end.
static bool at_end(HWND edit) {
  DWORD from = 0, to = 0;
  SendMessageW(edit, EM_GETSEL, (WPARAM)&from, (LPARAM)&to);
  return from == to && (int)to == GetWindowTextLengthW(edit);
}

static LRESULT CALLBACK edit_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR id, DWORD_PTR data) {
  App *a = (App *)data;
  if (msg == WM_KEYDOWN) {
    bool ours = wp == VK_UP || wp == VK_DOWN || wp == VK_PRIOR || wp == VK_NEXT || wp == VK_RETURN ||
                wp == VK_ESCAPE || (wp == VK_DELETE && at_end(hwnd));
    if (ours && keys_list(a, (UINT)wp)) return 0;
  }
  // Enter and Esc would beep in a single-line edit.
  if (msg == WM_CHAR && (wp == '\r' || wp == 0x1B || wp == '\t')) return 0;
  return DefSubclassProc(hwnd, msg, wp, lp);
}

void search_create(App *a) {
  a->search = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | ES_AUTOHSCROLL, 0, 0, 10, 10, a->hwnd, NULL,
                              a->instance, NULL);
  SendMessageW(a->search, EM_SETLIMITTEXT, 120, 0);
  SetWindowSubclass(a->search, edit_proc, SUBCLASS_ID, (DWORD_PTR)a);
  search_language_changed(a);
  search_place(a);
}

void search_place(App *a) {
  if (!a->search) return;
  Box b = layout_search(a);
  SendMessageW(a->search, WM_SETFONT, (WPARAM)a->search_font, FALSE);
  SetWindowPos(a->search, NULL, b.x, b.y, b.w, b.h, SWP_NOZORDER | SWP_NOACTIVATE);
  ShowWindow(a->search, a->view == POPUP_LIST ? SW_SHOW : SW_HIDE);
}

void search_clear(App *a) {
  SetWindowTextW(a->search, L"");
  a->query[0] = 0;
}

void search_changed(App *a) {
  wchar_t text[128];
  GetWindowTextW(a->search, text, 128);
  char *utf8 = wide_to_utf8(text, -1, NULL);
  snprintf(a->query, sizeof a->query, "%s", utf8);
  free(utf8);
  a->confirm.active = false;
  rows_view_refilter(a);
  a->top = 0;
  a->selected = a->visible_count ? 0 : -1;
  popup_fit(a);
}

LRESULT search_colors(App *a, HDC dc) {
  SetTextColor(dc, rgb(theme()->text));
  SetBkColor(dc, rgb(theme()->header));
  return (LRESULT)a->search_brush;
}

void search_theme_changed(App *a) {
  if (a->search_brush) DeleteObject(a->search_brush);
  a->search_brush = CreateSolidBrush(rgb(theme()->header));
  if (a->search) InvalidateRect(a->search, NULL, TRUE);
}

void search_language_changed(App *a) {
  wchar_t *hint = utf8_to_wide(strings()->search_hint, -1);
  SendMessageW(a->search, EM_SETCUEBANNER, TRUE, (LPARAM)hint);
  free(hint);
}
