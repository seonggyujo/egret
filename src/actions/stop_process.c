// Ending a process with TerminateProcess, after checking it is still the
// program the row showed.
#include <string.h>
#include <windows.h>

#include "actions/stop_parts.h"
#include "base/log.h"
#include "base/names.h"

#define EXIT_WAIT_MS 3000

// Whether h is still the program that was seen: same file name, same
// start time, when those were known.
static bool same_program(HANDLE h, const char *exe, uint64_t started) {
  if (exe && exe[0]) {
    wchar_t path[1024];
    char utf8[3072];
    DWORD n = sizeof path / sizeof path[0];
    if (QueryFullProcessImageNameW(h, 0, path, &n) &&
        WideCharToMultiByte(CP_UTF8, 0, path, -1, utf8, sizeof utf8, NULL, NULL) &&
        !name_equal(path_file_name(utf8), exe)) {
      return false;
    }
  }
  FILETIME created, exited, kernel, user;
  if (started && GetProcessTimes(h, &created, &exited, &kernel, &user)) {
    uint64_t t = ((uint64_t)created.dwHighDateTime << 32) | created.dwLowDateTime;
    if (t != started) return false;
  }
  return true;
}

StopResult stop_process(uint32_t pid, const char *exe, uint64_t started) {
  HANDLE h = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
  if (!h) {
    DWORD err = GetLastError();
    log_info("stop", "PID %u: cannot open (%lu)", pid, err);
    return err == ERROR_INVALID_PARAMETER ? STOP_GONE : err == ERROR_ACCESS_DENIED ? STOP_DENIED : STOP_FAILED;
  }
  StopResult r;
  if (!same_program(h, exe, started) || WaitForSingleObject(h, 0) == WAIT_OBJECT_0) {
    r = STOP_GONE;
  } else if (!TerminateProcess(h, 1)) {
    r = GetLastError() == ERROR_ACCESS_DENIED ? STOP_DENIED : STOP_FAILED;
  } else {
    r = WaitForSingleObject(h, EXIT_WAIT_MS) == WAIT_OBJECT_0 ? STOP_OK : STOP_FAILED;
  }
  CloseHandle(h);
  log_info("stop", "PID %u (%s): %d", pid, exe ? exe : "", (int)r);
  return r;
}
