// The stop thread and what its result says.
#include "ui/stopper.h"

#include <stdlib.h>

#include "actions/stop.h"
#include "app/strings.h"
#include "base/log.h"
#include "base/mem.h"
#include "ui/scanner.h"
#include "ui/tray.h"

static DWORD WINAPI stop_thread(void *param) {
  StopJob *job = param;
  job->result = job->admin ? stop_elevated(&job->row) : stop_listener(&job->row);
  if (!PostMessageW(job->hwnd, WM_APP_STOPPED, 0, (LPARAM)job)) free(job);
  return 0;
}

bool stopper_start(App *a, const Listener *row, bool admin) {
  if (a->stopping.active) return false;
  StopJob *job = xcalloc(1, sizeof *job);
  job->hwnd = a->hwnd;
  job->row = *row;
  job->admin = admin;
  HANDLE thread = CreateThread(NULL, 0, stop_thread, job, 0, NULL);
  if (!thread) {
    log_error("stopper", "CreateThread failed: %lu", GetLastError());
    free(job);
    return false;
  }
  CloseHandle(thread);
  a->stopping = (Pending){true, admin, row->port, row->pid, 0};
  log_info("stopper", "stopping port %u pid %lu%s", row->port, (unsigned long)row->pid, admin ? " (admin)" : "");
  InvalidateRect(a->hwnd, NULL, FALSE);
  return true;
}

static const char *outcome_text(int result, bool *error) {
  const Strings *s = strings();
  *error = result != STOP_OK && result != STOP_GONE;
  switch (result) {
    case STOP_OK: return s->stopped;
    case STOP_GONE: return s->already_gone;
    case STOP_DENIED: return s->needs_admin;
    case STOP_BLOCKED: return s->blocked_system;
    case STOP_NO_ENGINE: return s->no_engine;
    default: return s->stop_failed;
  }
}

void stopper_done(App *a, StopJob *job) {
  bool error = false;
  const char *outcome = outcome_text(job->result, &error);
  if (job->result == STOP_BLOCKED && job->row.kind == KIND_WSL) outcome = strings()->blocked_wsl;
  char text[256];
  format_stop_outcome(text, sizeof text, job->row.port, job->row.name, outcome);
  log_info("stopper", "%s", text);
  a->stopping.active = false;
  if (a->shown) app_note(a, text, error);
  else tray_notify(a, text);
  free(job);
  scanner_wake();
}
