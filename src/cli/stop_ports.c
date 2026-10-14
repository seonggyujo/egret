// Stopping what holds the ports named on the command line: show the
// targets, ask unless -y, stop each, say how it went.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "actions/stop.h"
#include "app/strings.h"
#include "base/mem.h"
#include "base/names.h"
#include "cli/cli_parts.h"
#include "cli/out.h"
#include "scan/scan.h"

#define EXIT_FAILED 1
#define EXIT_NO_LISTENER 2
#define EXIT_CANCELLED 3

static void print_target(const Listener *l) {
  char line[RAW_PATH + RAW_TEXT + 64];
  if (l->kind == KIND_CONTAINER) {
    snprintf(line, sizeof line, "%u  %s (docker)  %s", (unsigned)l->port, l->name, l->image);
  } else {
    snprintf(line, sizeof line, "%u  %s (PID %u)  %s", (unsigned)l->port, l->name, (unsigned)l->pid, l->folder);
  }
  out_line(line);
}

static bool confirmed(void) {
  char answer[16];
  out_text(strings()->cli_confirm);
  if (!in_line(answer, sizeof answer)) return false;
  char *a = answer;
  while (*a == ' ') a++;
  for (size_t n = strlen(a); n && a[n - 1] == ' ';) a[--n] = 0;
  return name_equal(a, "y") || name_equal(a, "yes");
}

static const char *outcome(StopResult r, const Listener *l) {
  const Strings *s = strings();
  switch (r) {
    case STOP_OK: return s->stopped;
    case STOP_GONE: return s->already_gone;
    case STOP_DENIED: return s->denied_cli;
    case STOP_BLOCKED: return l->kind == KIND_WSL ? s->blocked_wsl : s->blocked_system;
    case STOP_NO_ENGINE: return s->no_engine;
    default: return s->stop_failed;
  }
}

// The rows on the asked ports, in the order asked. Ports nobody listens on
// are reported and make *code EXIT_NO_LISTENER.
static int find_targets(const CliArgs *a, const ListenerList *rows, const Listener **out, int *code) {
  int n = 0;
  for (int p = 0; p < a->port_count; p++) {
    bool found = false;
    for (int i = 0; i < rows->count; i++) {
      if (rows->items[i].port != a->ports[p]) continue;
      out[n++] = &rows->items[i];
      found = true;
    }
    if (found) continue;
    char msg[128];
    format_cli_no_listener(msg, sizeof msg, a->ports[p]);
    err_line(msg);
    *code = EXIT_NO_LISTENER;
  }
  return n;
}

int cli_stop(const CliArgs *a) {
  ListenerList rows = {0};
  scan_run(&rows);
  const Listener **targets = xcalloc((size_t)rows.count + 1, sizeof *targets);
  int code = 0, n = find_targets(a, &rows, targets, &code);
  for (int i = 0; i < n; i++) print_target(targets[i]);
  if (n > 0 && !a->yes && !confirmed()) {
    out_line(strings()->cli_cancelled);
    code = EXIT_CANCELLED;
    n = 0;
  }
  bool failed = false;
  for (int i = 0; i < n; i++) {
    StopResult r = stop_listener(targets[i]);
    char line[256];
    snprintf(line, sizeof line, "%u  %s", (unsigned)targets[i]->port, outcome(r, targets[i]));
    out_line(line);
    failed |= r != STOP_OK && r != STOP_GONE;
  }
  free(targets);
  listener_list_free(&rows);
  return failed ? EXIT_FAILED : code;
}
