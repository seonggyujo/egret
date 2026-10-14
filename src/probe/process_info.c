// Opening a process twice: once with the limited query right that most
// processes grant (path, start time), once with the right to read its
// memory (current folder).
#include "probe/process_info.h"

#include <string.h>
#include <windows.h>

#include "probe/process_cwd.h"

static void read_path_and_time(HANDLE h, RawProcInfo *out) {
  wchar_t path[1024];
  DWORD n = sizeof path / sizeof path[0];
  if (QueryFullProcessImageNameW(h, 0, path, &n) &&
      !WideCharToMultiByte(CP_UTF8, 0, path, -1, out->path, sizeof out->path, NULL, NULL)) {
    out->path[0] = 0;
  }
  FILETIME created, exited, kernel, user;
  if (GetProcessTimes(h, &created, &exited, &kernel, &user)) {
    out->started = ((uint64_t)created.dwHighDateTime << 32) | created.dwLowDateTime;
  }
}

void probe_process_info(uint32_t pid, RawProcInfo *out) {
  memset(out, 0, sizeof *out);
  out->pid = pid;
  if (pid == 0 || pid == 4) return;  // the idle process and the kernel
  HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
  if (!h) return;
  out->opened = true;
  read_path_and_time(h, out);
  CloseHandle(h);
  h = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
  if (!h) return;
  if (!process_cwd_read(h, out->cwd, sizeof out->cwd)) out->cwd[0] = 0;
  CloseHandle(h);
}

void probe_windows_dir(char *out, size_t cap) {
  wchar_t dir[MAX_PATH];
  UINT n = GetWindowsDirectoryW(dir, MAX_PATH);
  if (n == 0 || n >= MAX_PATH || !WideCharToMultiByte(CP_UTF8, 0, dir, -1, out, (int)cap, NULL, NULL)) out[0] = 0;
}
