// egret.com: the command line. cmd and PowerShell run it first when
// "egret" is typed. Without arguments (or with --tray) it starts the tray
// app, egret.exe beside it, and returns.
#include <string.h>
#include <windows.h>
// After windows.h, which defines what shellapi.h uses.
#include <shellapi.h>

#include "actions/stop.h"
#include "app/autostart.h"
#include "app/exe_path.h"
#include "base/args.h"
#include "base/log.h"
#include "cli/cli.h"

static int start_tray_app(bool tray_flag) {
  wchar_t exe[MAX_PATH];
  if (!exe_path_gui(exe, MAX_PATH)) return 1;
  INT_PTR r = (INT_PTR)ShellExecuteW(NULL, L"open", exe, tray_flag ? L"" TRAY_FLAG : NULL, NULL, SW_SHOWNORMAL);
  return r > 32 ? 0 : 1;
}

int main(void) {
  int argc;
  char **argv = args_utf8(&argc);
  log_set_level(LOG_ERROR);  // quiet unless EGRET_LOG asks for more
  log_init();
  if (argc > 1 && strcmp(argv[1], ADMIN_STOP_FLAG) == 0) return stop_admin_main(argc, argv);
  if (cli_wants(argc, argv)) return cli_main(argc, argv);
  return start_tray_app(argc > 1);
}
