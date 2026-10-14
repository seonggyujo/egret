// Stopping a Windows service through the service manager. Without
// administrator rights the manager refuses, which becomes STOP_DENIED.
#include <stdlib.h>
#include <windows.h>

#include "actions/stop_parts.h"
#include "base/log.h"
#include "base/utf.h"

#define STOP_WAIT_MS 20000
#define POLL_MS 250

static StopResult from_error(DWORD err) {
  switch (err) {
    case ERROR_ACCESS_DENIED: return STOP_DENIED;
    case ERROR_SERVICE_DOES_NOT_EXIST:
    case ERROR_SERVICE_NOT_ACTIVE: return STOP_GONE;
    default: return STOP_FAILED;
  }
}

static StopResult wait_stopped(SC_HANDLE svc) {
  SERVICE_STATUS st;
  for (int waited = 0; waited < STOP_WAIT_MS; waited += POLL_MS) {
    if (!QueryServiceStatus(svc, &st)) return STOP_FAILED;
    if (st.dwCurrentState == SERVICE_STOPPED) return STOP_OK;
    Sleep(POLL_MS);
  }
  return STOP_FAILED;
}

StopResult stop_service(const char *name) {
  SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
  if (!scm) return from_error(GetLastError());
  wchar_t *wname = utf8_to_wide(name, -1);
  SC_HANDLE svc = OpenServiceW(scm, wname, SERVICE_STOP | SERVICE_QUERY_STATUS);
  free(wname);
  StopResult r;
  SERVICE_STATUS st;
  if (!svc) r = from_error(GetLastError());
  else if (!ControlService(svc, SERVICE_CONTROL_STOP, &st)) r = from_error(GetLastError());
  else r = wait_stopped(svc);
  if (svc) CloseServiceHandle(svc);
  CloseServiceHandle(scm);
  log_info("stop", "service %s: %d", name, (int)r);
  return r;
}
