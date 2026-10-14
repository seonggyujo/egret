// Walking a Toolhelp process snapshot.
#include "probe/processes.h"

#include <windows.h>
// After windows.h, which defines what tlhelp32.h uses.
#include <tlhelp32.h>

#include "base/log.h"
#include "base/mem.h"

bool probe_processes(RawScan *raw) {
  HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snap == INVALID_HANDLE_VALUE) {
    log_warn("processes", "no process snapshot: %lu", GetLastError());
    return false;
  }
  int cap = 256;
  raw->procs = xmalloc(sizeof *raw->procs * (size_t)cap);
  PROCESSENTRY32W e = {.dwSize = sizeof e};
  for (BOOL more = Process32FirstW(snap, &e); more; more = Process32NextW(snap, &e)) {
    if (raw->proc_count == cap) {
      cap *= 2;
      raw->procs = xrealloc(raw->procs, sizeof *raw->procs * (size_t)cap);
    }
    RawProcess *p = &raw->procs[raw->proc_count++];
    p->pid = e.th32ProcessID;
    p->parent = e.th32ParentProcessID;
    if (!WideCharToMultiByte(CP_UTF8, 0, e.szExeFile, -1, p->exe, sizeof p->exe, NULL, NULL)) p->exe[0] = 0;
  }
  CloseHandle(snap);
  return true;
}
