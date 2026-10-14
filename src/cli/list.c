// The --list table: PORT PID PROCESS UP FOLDER TAGS, each column as wide
// as its widest cell.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app/hidden.h"
#include "app/strings.h"
#include "base/buf.h"
#include "base/clock.h"
#include "base/mem.h"
#include "cli/cli_parts.h"
#include "cli/columns.h"
#include "cli/out.h"
#include "model/filter.h"
#include "model/text.h"
#include "scan/scan.h"

#define COLS 6

typedef struct {
  char cells[COLS][RAW_PATH];
} Line;

static void add_tag(char *out, size_t cap, bool on, const char *tag) {
  if (!on) return;
  size_t n = strlen(out);
  snprintf(out + n, cap - n, "%s%s", n ? " " : "", tag);
}

static void tags(const Listener *l, char *out, size_t cap) {
  out[0] = 0;
  add_tag(out, cap, l->heron, "heron-c");
  add_tag(out, cap, l->kind == KIND_SERVICE, "service");
  add_tag(out, cap, l->kind == KIND_CONTAINER, "docker");
  add_tag(out, cap, l->kind == KIND_WSL, "wsl");
  add_tag(out, cap, l->kind == KIND_SYSTEM, "system");
  add_tag(out, cap, l->lan, "lan");
}

static void fill(Line *line, const Listener *l, uint64_t now) {
  bool container = l->kind == KIND_CONTAINER;
  const char *folder = container ? l->image : l->folder;
  snprintf(line->cells[0], RAW_PATH, "%u", (unsigned)l->port);
  if (container) snprintf(line->cells[1], RAW_PATH, "-");
  else snprintf(line->cells[1], RAW_PATH, "%u", (unsigned)l->pid);
  snprintf(line->cells[2], RAW_PATH, "%s", l->name[0] ? l->name : "-");
  text_uptime(line->cells[3], RAW_PATH, l->started, now);
  snprintf(line->cells[4], RAW_PATH, "%s", folder[0] ? folder : "-");
  tags(l, line->cells[5], RAW_PATH);
}

static void print(const Line *lines, int n) {
  int width[COLS] = {0};
  for (int i = 0; i < n; i++) {
    for (int c = 0; c < COLS; c++) {
      int w = text_columns(lines[i].cells[c]);
      if (w > width[c]) width[c] = w;
    }
  }
  Buf b = {0};
  for (int i = 0; i < n; i++) {
    buf_clear(&b);
    for (int c = 0; c < COLS; c++) {
      buf_append_str(&b, lines[i].cells[c]);
      for (int pad = text_columns(lines[i].cells[c]); c + 1 < COLS && pad < width[c] + 2; pad++) buf_append(&b, " ", 1);
    }
    while (b.len && b.data[b.len - 1] == ' ') b.len--;
    buf_append(&b, "", 1);
    out_line(b.data);
  }
  buf_free(&b);
}

int cli_list(bool all) {
  static const char *const HEADER[COLS] = {"PORT", "PID", "PROCESS", "UP", "FOLDER", "TAGS"};
  ListenerList rows = {0};
  scan_run(&rows);
  HiddenList hidden;
  hidden_load(&hidden);
  const char *names[HIDDEN_MAX];
  int hidden_count = hidden_pointers(&hidden, names, HIDDEN_MAX);
  Line *lines = xcalloc((size_t)rows.count + 1, sizeof *lines);
  for (int c = 0; c < COLS; c++) snprintf(lines[0].cells[c], RAW_PATH, "%s", HEADER[c]);
  int n = 1;
  uint64_t now = clock_filetime_now();
  for (int i = 0; i < rows.count; i++) {
    if (all || row_is_default(&rows.items[i], names, hidden_count)) fill(&lines[n++], &rows.items[i], now);
  }
  if (n == 1) out_line(strings()->empty_title);
  else print(lines, n);
  free(lines);
  listener_list_free(&rows);
  return 0;
}
