// One look at the machine: every probe in turn, then the rows.
#include "scan/scan.h"

#include "base/mem.h"
#include "docker/docker.h"
#include "model/rows.h"
#include "probe/process_info.h"
#include "probe/processes.h"
#include "probe/services.h"
#include "probe/sockets.h"

static bool seen(const RawScan *raw, uint32_t pid) {
  for (int i = 0; i < raw->info_count; i++) {
    if (raw->infos[i].pid == pid) return true;
  }
  return false;
}

// One RawProcInfo per PID that owns a socket.
static void probe_listening_processes(RawScan *raw) {
  raw->infos = xcalloc((size_t)raw->socket_count + 1, sizeof *raw->infos);
  for (int i = 0; i < raw->socket_count; i++) {
    uint32_t pid = raw->sockets[i].pid;
    if (!seen(raw, pid)) probe_process_info(pid, &raw->infos[raw->info_count++]);
  }
}

void scan_run(ListenerList *out) {
  RawScan raw = {0};
  probe_windows_dir(raw.windows_dir, sizeof raw.windows_dir);
  probe_sockets(&raw);
  probe_processes(&raw);
  probe_listening_processes(&raw);
  probe_services(&raw);
  docker_list(&raw);
  rows_build(&raw, out);
  raw_scan_free(&raw);
}
