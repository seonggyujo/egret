// The tooltip control and what each target says.
#include "ui/tooltip.h"

#include <commctrl.h>
#include <string.h>
#include <wchar.h>

#include "actions/stop.h"
#include "app/strings.h"
#include "base/log.h"
#include "ui/layout.h"
#include "ui/list_input.h"

#define TOOL_ID 1

void tooltip_create(App *a) {
  INITCOMMONCONTROLSEX init = {sizeof init, ICC_BAR_CLASSES};
  InitCommonControlsEx(&init);
  a->tooltip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, a->hwnd, NULL, NULL, NULL);
  if (!a->tooltip) {
    log_warn("tooltip", "no tooltip control: %lu", GetLastError());
    return;
  }
  TTTOOLINFOW tool = {.cbSize = sizeof tool, .uFlags = TTF_SUBCLASS, .hwnd = a->hwnd, .uId = TOOL_ID,
                      .lpszText = LPSTR_TEXTCALLBACKW};
  SendMessageW(a->tooltip, TTM_ADDTOOLW, 0, (LPARAM)&tool);
  SendMessageW(a->tooltip, TTM_SETMAXTIPWIDTH, 0, (LPARAM)(int)(420 * a->scale));
}

static const char *tag_tip(const Listener *l) {
  const Strings *s = strings();
  if (l->kind == KIND_WSL) return s->tip_wsl;
  if (l->lan && l->kind != KIND_SYSTEM) return s->tip_lan;
  return l->heron ? s->tip_heron : NULL;
}

// What the thing under (x, y) explains, and its box.
static const char *tip_at(const App *a, int x, int y, Box *box) {
  const Strings *s = strings();
  if (a->view == POPUP_LIST && box_contains(*box = layout_gear(a), x, y)) return s->tip_settings;
  if (a->view == POPUP_SETTINGS && box_contains(*box = layout_back(a), x, y)) return s->back;
  int vi = -1, part = list_hit(a, x, y, &vi);
  const Listener *l = app_row(a, vi);
  if (!l) return NULL;
  RowParts p;
  layout_row_parts(a, layout_row(a, vi), &p);
  switch (part) {
    case PART_BROWSER: *box = p.browser; return s->tip_browser;
    case PART_FOLDER: *box = p.folder_button; return s->tip_folder;
    case PART_STOP: *box = p.stop; return stop_needs_admin(l) ? s->tip_stop_admin : s->tip_stop;
    case PART_TAGS: *box = p.tags; return tag_tip(l);
    case PART_FOLDER_TEXT:
      *box = p.folder;
      return l->folder[0] ? l->folder : l->path[0] ? l->path : NULL;
    default: return NULL;
  }
}

void tooltip_mouse_move(App *a, int x, int y) {
  if (!a->tooltip) return;
  Box box = {0};
  const char *text = tip_at(a, x, y, &box);
  if (!text) box = (Box){0};
  wchar_t wide[512] = L"";
  if (text) MultiByteToWideChar(CP_UTF8, 0, text, -1, wide, 512);
  if (memcmp(&box, &a->tip_box, sizeof box) == 0 && wcscmp(wide, a->tip_text) == 0) return;
  a->tip_box = box;
  wcscpy(a->tip_text, wide);
  SendMessageW(a->tooltip, TTM_POP, 0, 0);
  // An empty rectangle when there is nothing to explain.
  TTTOOLINFOW tool = {.cbSize = sizeof tool, .hwnd = a->hwnd, .uId = TOOL_ID,
                      .rect = {box.x, box.y, box.x + box.w, box.y + box.h}};
  SendMessageW(a->tooltip, TTM_NEWTOOLRECTW, 0, (LPARAM)&tool);
}

bool tooltip_notify(App *a, LPARAM lp) {
  NMHDR *n = (NMHDR *)lp;
  if (!a->tooltip || n->hwndFrom != a->tooltip) return false;
  if (n->code == TTN_GETDISPINFOW) ((NMTTDISPINFOW *)lp)->lpszText = a->tip_text;
  return true;
}
