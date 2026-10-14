// Stopping with administrator rights: egret.exe started again through the
// UAC prompt does the work and hands back the StopResult as its exit code.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
// After windows.h, which defines what shellapi.h uses.
#include <shellapi.h>

#include "actions/stop.h"
#include "actions/stop_parts.h"
#include "app/exe_path.h"
#include "base/log.h"
#include "base/utf.h"

#define ELEVATED_WAIT_MS 60000

StopResult stop_elevated(const Listener *l) {
  if (l->kind == KIND_SYSTEM || l->kind == KIND_WSL) return STOP_BLOCKED;
  if (l->kind == KIND_CONTAINER) return stop_listener(l);  // Docker needs no elevation
  wchar_t exe[MAX_PATH], params[300];
  if (!exe_path_gui(exe, MAX_PATH)) return STOP_FAILED;
  wchar_t *service = utf8_to_wide(l->kind == KIND_SERVICE ? l->service : "", -1);
  if (service[0]) swprintf(params, 300, L"%hs %u \"%ls\"", ADMIN_STOP_FLAG, l->pid, service);
  else swprintf(params, 300, L"%hs %u", ADMIN_STOP_FLAG, l->pid);
  free(service);
  SHELLEXECUTEINFOW sei = {.cbSize = sizeof sei, .fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC,
                           .lpVerb = L"runas", .lpFile = exe, .lpParameters = params, .nShow = SW_HIDE};
  if (!ShellExecuteExW(&sei)) {
    DWORD err = GetLastError();
    log_info("stop", "elevation refused: %lu", err);
    return err == ERROR_CANCELLED ? STOP_DENIED : STOP_FAILED;
  }
  DWORD code = STOP_FAILED;
  if (!sei.hProcess || WaitForSingleObject(sei.hProcess, ELEVATED_WAIT_MS) != WAIT_OBJECT_0 ||
      !GetExitCodeProcess(sei.hProcess, &code) || code > STOP_FAILED) {
    code = STOP_FAILED;
  }
  if (sei.hProcess) CloseHandle(sei.hProcess);
  return (StopResult)code;
}

int stop_admin_main(int argc, char **argv) {
  int at = 1;
  while (at < argc && strcmp(argv[at], ADMIN_STOP_FLAG) != 0) at++;
  if (at + 1 >= argc) return STOP_FAILED;
  char *end;
  unsigned long pid = strtoul(argv[at + 1], &end, 10);
  if (*end || pid == 0) return STOP_FAILED;
  const char *service = at + 2 < argc ? argv[at + 2] : "";
  return service[0] ? (int)stop_service(service) : (int)stop_process((uint32_t)pid, "", 0);
}
