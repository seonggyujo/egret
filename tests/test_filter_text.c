// Which rows show, search matching, uptime and short path texts.
#include <string.h>

#include "check.h"
#include "model/filter.h"
#include "model/text.h"

static Listener row(uint16_t port, ListenerKind kind, const char *name, const char *exe, const char *folder) {
  Listener l;
  memset(&l, 0, sizeof l);
  l.port = port;
  l.kind = kind;
  strcpy(l.name, name);
  strcpy(l.exe, exe);
  strcpy(l.folder, folder);
  return l;
}

void test_filter(void) {
  const char *hidden[] = {"Discord.exe"};
  Listener site = row(5173, KIND_PROCESS, "node", "node.exe", "C:\\code\\Egret-Site");
  Listener discord = row(6463, KIND_PROCESS, "Discord", "discord.EXE", "C:\\Discord");
  Listener rpc = row(135, KIND_SYSTEM, "svchost", "svchost.exe", "");
  CHECK(row_is_default(&site, hidden, 1));
  CHECK(!row_is_default(&discord, hidden, 1));
  CHECK(!row_is_default(&rpc, hidden, 1));
  CHECK(row_matches(&site, "51"));
  CHECK(row_matches(&site, " 5173 "));
  CHECK(!row_matches(&site, "73"));
  CHECK(row_matches(&site, "egret-site"));
  CHECK(row_matches(&site, "NODE"));
  CHECK(row_matches(&site, ""));
  CHECK(row_matches(&site, NULL));
  CHECK(!row_matches(&site, "python"));
  RowFilter f = {"", false, hidden, 1};
  CHECK(row_visible(&site, &f) && !row_visible(&discord, &f) && !row_visible(&rpc, &f));
  f.show_all = true;
  CHECK(row_visible(&discord, &f) && row_visible(&rpc, &f));
  f.query = "svc";
  CHECK(row_visible(&rpc, &f) && !row_visible(&site, &f));
  ListenerList list = {0};
  *listener_list_add(&list) = site;
  *listener_list_add(&list) = discord;
  *listener_list_add(&list) = rpc;
  int shown, tucked;
  rows_count(&list, hidden, 1, &shown, &tucked);
  CHECK(shown == 1 && tucked == 2);
  listener_list_free(&list);
}

void test_texts(void) {
  char out[64];
  const uint64_t s = 10000000ULL;
  text_uptime(out, sizeof out, 0, 100 * s);
  CHECK_STR(out, "-");
  text_uptime(out, sizeof out, 200 * s, 100 * s);
  CHECK_STR(out, "-");
  text_uptime(out, sizeof out, 1, 1 + 41 * s);
  CHECK_STR(out, "41s");
  text_uptime(out, sizeof out, 1, 1 + 12 * 60 * s);
  CHECK_STR(out, "12m");
  text_uptime(out, sizeof out, 1, 1 + (2 * 3600 + 13 * 60) * s);
  CHECK_STR(out, "2h 13m");
  text_uptime(out, sizeof out, 1, 1 + (3 * 86400 + 4 * 3600 + 59) * s);
  CHECK_STR(out, "3d 4h");
  text_short_path(out, sizeof out, "C:\\Users\\home\\IdeaProjects\\egret", "c:\\users\\HOME");
  CHECK_STR(out, "~\\IdeaProjects\\egret");
  text_short_path(out, sizeof out, "C:\\Users\\homer\\x", "C:\\Users\\home");
  CHECK_STR(out, "C:\\Users\\homer\\x");
  text_short_path(out, sizeof out, "C:\\Users\\home", "C:\\Users\\home\\");
  CHECK_STR(out, "~");
}
