// Deciding what holds a port (process, service, container, WSL or system)
// and what the row shows for it, by the rules in model/rows.h.
#include <string.h>

#include "base/names.h"
#include "model/rows_parts.h"

// Programs that hold a port on behalf of a container.
static const char *const FORWARDERS[] = {"com.docker.backend.exe", "com.docker.proxy.exe", "vpnkit.exe",
                                         "dockerd.exe", "docker-proxy.exe", "wslrelay.exe", "gvproxy.exe"};

#define HERON_EXE "heron-c.exe"
#define MAX_ANCESTORS 16

static const RawProcess *find_proc(const RawScan *raw, uint32_t pid) {
  for (int i = 0; i < raw->proc_count; i++) {
    if (raw->procs[i].pid == pid) return &raw->procs[i];
  }
  return NULL;
}

static const RawProcInfo *find_info(const RawScan *raw, uint32_t pid) {
  for (int i = 0; i < raw->info_count; i++) {
    if (raw->infos[i].pid == pid) return &raw->infos[i];
  }
  return NULL;
}

static const RawService *find_service(const RawScan *raw, uint32_t pid) {
  for (int i = 0; pid && i < raw->service_count; i++) {
    if (raw->services[i].pid == pid) return &raw->services[i];
  }
  return NULL;
}

static bool is_forwarder(const char *exe) {
  for (size_t i = 0; i < sizeof FORWARDERS / sizeof FORWARDERS[0]; i++) {
    if (name_equal(exe, FORWARDERS[i])) return true;
  }
  return false;
}

static const RawContainer *container_on(const RawScan *raw, uint16_t port) {
  for (int i = 0; i < raw->container_count; i++) {
    const RawContainer *c = &raw->containers[i];
    for (int k = 0; k < c->port_count; k++) {
      if (c->ports[k].public_port == port) return c;
    }
  }
  return NULL;
}

// The service the process runs, or its parent runs, when that service's
// program lies outside the Windows folder.
static const RawService *own_service(const RawScan *raw, uint32_t pid, const RawProcess *proc) {
  const RawService *s = find_service(raw, pid);
  if (!s && proc) s = find_service(raw, proc->parent);
  if (!s || !s->binary[0] || path_inside(s->binary, raw->windows_dir)) return NULL;
  return s;
}

static bool under_heron(const RawScan *raw, const RawProcess *proc) {
  uint32_t pid = proc ? proc->parent : 0;
  for (int depth = 0; depth < MAX_ANCESTORS && pid > 4; depth++) {
    const RawProcess *p = find_proc(raw, pid);
    if (!p) return false;
    if (name_equal(p->exe, HERON_EXE)) return true;
    if (p->parent == pid) return false;
    pid = p->parent;
  }
  return false;
}

// "node.exe" shows as "node".
static void display_name(char *out, size_t cap, const char *exe) {
  text_copy(out, cap, exe);
  size_t n = strlen(out);
  if (n > 4 && name_equal(out + n - 4, ".exe")) out[n - 4] = 0;
}

// The working folder, unless it is none or inside the Windows folder
// (programs started at sign-in run in System32): then the program's folder.
static void take_info(Listener *row, const RawProcInfo *info, const char *windows_dir) {
  if (!info || !info->opened || !info->path[0]) return;
  row->opened = true;
  row->started = info->started;
  text_copy(row->path, sizeof row->path, info->path);
  if (info->cwd[0] && !path_inside(info->cwd, windows_dir)) text_copy(row->folder, sizeof row->folder, info->cwd);
  else path_folder(info->path, row->folder, sizeof row->folder);
}

static void as_container(Listener *row, const RawContainer *c) {
  row->kind = KIND_CONTAINER;
  text_copy(row->name, sizeof row->name, c->name);
  text_copy(row->container_id, sizeof row->container_id, c->id);
  text_copy(row->image, sizeof row->image, c->image);
  row->exe[0] = row->folder[0] = 0;
  row->started = c->created;
}

// A service shows its program's folder: services start in System32, so
// their working folder says nothing.
static void as_service(Listener *row, const RawService *s) {
  row->kind = KIND_SERVICE;
  text_copy(row->service, sizeof row->service, s->name);
  path_folder(row->opened ? row->path : s->binary, row->folder, sizeof row->folder);
}

void rows_classify(const RawScan *raw, Listener *row) {
  const RawProcess *proc = find_proc(raw, row->pid);
  if (proc) text_copy(row->exe, sizeof row->exe, proc->exe);
  display_name(row->name, sizeof row->name, row->exe);
  take_info(row, find_info(raw, row->pid), raw->windows_dir);
  row->heron = under_heron(raw, proc);
  const RawContainer *c = is_forwarder(row->exe) ? container_on(raw, row->port) : NULL;
  if (c) {
    as_container(row, c);
    return;
  }
  if (name_equal(row->exe, "wslrelay.exe")) {
    row->kind = KIND_WSL;
    return;
  }
  if (row->pid == 0 || row->pid == 4 || (row->opened && path_inside(row->path, raw->windows_dir))) {
    row->kind = KIND_SYSTEM;
    return;
  }
  const RawService *s = own_service(raw, row->pid, proc);
  if (s) as_service(row, s);
  else row->kind = row->opened ? KIND_PROCESS : KIND_SYSTEM;
}
