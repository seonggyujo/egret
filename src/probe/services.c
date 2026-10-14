// Asking the service manager which services run in the listening processes
// (or their parents) and what program each one's command line starts.
#include "probe/services.h"

#include <stdlib.h>
#include <windows.h>

#include "base/log.h"
#include "base/mem.h"
#include "probe/service_program.h"

static uint32_t parent_of(const RawScan *raw, uint32_t pid) {
  for (int i = 0; i < raw->proc_count; i++) {
    if (raw->procs[i].pid == pid) return raw->procs[i].parent;
  }
  return 0;
}

static bool wanted(const RawScan *raw, uint32_t pid) {
  for (int i = 0; pid && i < raw->socket_count; i++) {
    uint32_t owner = raw->sockets[i].pid;
    if (owner == pid || parent_of(raw, owner) == pid) return true;
  }
  return false;
}

// The command line the service manager keeps for the service, environment
// variables expanded, as UTF-8.
static bool command_line(SC_HANDLE scm, const wchar_t *name, char *out, size_t cap) {
  SC_HANDLE svc = OpenServiceW(scm, name, SERVICE_QUERY_CONFIG);
  if (!svc) return false;
  DWORD need = 0;
  QueryServiceConfigW(svc, NULL, 0, &need);
  QUERY_SERVICE_CONFIGW *cfg = need ? xmalloc(need) : NULL;
  bool ok = cfg && QueryServiceConfigW(svc, cfg, need, &need) && cfg->lpBinaryPathName;
  if (ok) {
    wchar_t expanded[2048];
    DWORD n = ExpandEnvironmentStringsW(cfg->lpBinaryPathName, expanded, 2048);
    ok = n > 0 && n <= 2048 && WideCharToMultiByte(CP_UTF8, 0, expanded, -1, out, (int)cap, NULL, NULL) > 0;
  }
  free(cfg);
  CloseServiceHandle(svc);
  return ok;
}

static void add(RawScan *raw, int *cap, SC_HANDLE scm, const ENUM_SERVICE_STATUS_PROCESSW *e) {
  char command[2048];
  if (!command_line(scm, e->lpServiceName, command, sizeof command)) return;
  if (raw->service_count == *cap) {
    *cap = *cap ? *cap * 2 : 8;
    raw->services = xrealloc(raw->services, sizeof *raw->services * (size_t)*cap);
  }
  RawService *s = &raw->services[raw->service_count++];
  s->pid = e->ServiceStatusProcess.dwProcessId;
  if (!WideCharToMultiByte(CP_UTF8, 0, e->lpServiceName, -1, s->name, sizeof s->name, NULL, NULL)) s->name[0] = 0;
  service_program(command, raw->windows_dir, s->binary, sizeof s->binary);
}

// The whole list of running Win32 services, malloc'd.
static ENUM_SERVICE_STATUS_PROCESSW *enumerate(SC_HANDLE scm, DWORD *count) {
  DWORD need = 0, resume = 0;
  *count = 0;
  EnumServicesStatusExW(scm, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_ACTIVE, NULL, 0, &need, count,
                        &resume, NULL);
  if (GetLastError() != ERROR_MORE_DATA) return NULL;
  need += 4096;  // services may start between the two calls
  BYTE *buf = xmalloc(need);
  resume = 0;
  if (!EnumServicesStatusExW(scm, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_ACTIVE, buf, need, &need,
                             count, &resume, NULL)) {
    log_warn("services", "EnumServicesStatusExW failed: %lu", GetLastError());
    *count = 0;
  }
  return (ENUM_SERVICE_STATUS_PROCESSW *)buf;
}

bool probe_services(RawScan *raw) {
  SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_ENUMERATE_SERVICE);
  if (!scm) {
    log_warn("services", "no service manager: %lu", GetLastError());
    return false;
  }
  DWORD count = 0;
  ENUM_SERVICE_STATUS_PROCESSW *list = enumerate(scm, &count);
  int cap = 0;
  for (DWORD i = 0; i < count; i++) {
    if (wanted(raw, list[i].ServiceStatusProcess.dwProcessId)) add(raw, &cap, scm, &list[i]);
  }
  free(list);
  CloseServiceHandle(scm);
  return true;
}
