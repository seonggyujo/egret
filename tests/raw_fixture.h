// Building small RawScans by hand for the rows tests.
#pragma once

#include <stdbool.h>
#include <string.h>

#include "model/raw.h"

static inline RawSocket fx_socket(uint16_t port, uint32_t pid, bool ipv6, bool loopback, bool wildcard) {
  RawSocket s = {port, pid, ipv6, loopback, wildcard};
  return s;
}

static inline RawProcess fx_proc(uint32_t pid, uint32_t parent, const char *exe) {
  RawProcess p = {pid, parent, ""};
  strcpy(p.exe, exe);
  return p;
}

// An opened process; cwd may be "".
static inline RawProcInfo fx_info(uint32_t pid, const char *path, const char *cwd) {
  RawProcInfo i;
  memset(&i, 0, sizeof i);
  i.pid = pid;
  i.opened = true;
  i.started = 1000;
  strcpy(i.path, path);
  strcpy(i.cwd, cwd);
  return i;
}

static inline RawService fx_service(uint32_t pid, const char *name, const char *binary) {
  RawService s;
  memset(&s, 0, sizeof s);
  s.pid = pid;
  strcpy(s.name, name);
  strcpy(s.binary, binary);
  return s;
}

// The arrays stay the caller's; nothing here is freed.
#define FX_SCAN(raw, socks, procs, infos)                         \
  RawScan raw;                                                    \
  memset(&raw, 0, sizeof raw);                                    \
  raw.sockets = socks;                                            \
  raw.socket_count = (int)(sizeof socks / sizeof socks[0]);       \
  raw.procs = procs;                                              \
  raw.proc_count = (int)(sizeof procs / sizeof procs[0]);         \
  raw.infos = infos;                                              \
  raw.info_count = (int)(sizeof infos / sizeof infos[0]);         \
  strcpy(raw.windows_dir, "C:\\WINDOWS")
