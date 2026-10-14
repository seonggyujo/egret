// Opening, stopping and the confirm row.
#include "ui/row_actions.h"

#include "actions/open.h"
#include "actions/stop.h"
#include "app/strings.h"
#include "ui/stopper.h"

#define CONFIRM_MS 3000

bool row_can_browse(const Listener *l) { return l->kind != KIND_SYSTEM; }

bool row_has_folder(const Listener *l) { return l->kind != KIND_CONTAINER && l->folder[0]; }

bool row_can_stop(const Listener *l) { return l->kind != KIND_SYSTEM && l->kind != KIND_WSL; }

void row_open_browser(App *a, int vi) {
  const Listener *l = app_row(a, vi);
  if (l && row_can_browse(l)) open_browser(l->port);
}

void row_open_folder(App *a, int vi) {
  const Listener *l = app_row(a, vi);
  if (l && row_has_folder(l)) open_folder(l->folder);
}

bool row_is_confirming(const App *a, const Listener *l) {
  return a->confirm.active && a->confirm.port == l->port && a->confirm.pid == l->pid;
}

bool row_is_stopping(const App *a, const Listener *l) {
  return a->stopping.active && a->stopping.port == l->port && a->stopping.pid == l->pid;
}

void row_ask_stop(App *a, int vi) {
  const Listener *l = app_row(a, vi);
  if (!l || row_is_stopping(a, l)) return;
  if (!row_can_stop(l)) {
    char text[256];
    const char *why = l->kind == KIND_WSL ? strings()->blocked_wsl : strings()->blocked_system;
    format_stop_outcome(text, sizeof text, l->port, l->name, why);
    app_note(a, text, true);
    return;
  }
  if (row_is_confirming(a, l)) {
    row_confirm_stop(a);
    return;
  }
  a->confirm = (Pending){true, stop_needs_admin(l), l->port, l->pid, GetTickCount64() + CONFIRM_MS};
  a->selected = vi;
  InvalidateRect(a->hwnd, NULL, FALSE);
}

void row_confirm_stop(App *a) {
  if (!a->confirm.active) return;
  a->confirm.active = false;
  for (int vi = 0; vi < a->visible_count; vi++) {
    const Listener *l = app_row(a, vi);
    if (l->port == a->confirm.port && l->pid == a->confirm.pid) {
      if (!stopper_start(a, l, a->confirm.admin)) app_note(a, strings()->stopping, false);
      break;
    }
  }
  InvalidateRect(a->hwnd, NULL, FALSE);
}

void row_cancel_stop(App *a) {
  a->confirm.active = false;
  InvalidateRect(a->hwnd, NULL, FALSE);
}
