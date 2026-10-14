// egret.exe, the tray app. With command line arguments (egret.exe --list)
// it runs the command line in the terminal that started it, and with
// --admin-stop it is the elevated helper. Otherwise it starts the tray, or,
// when egret already runs, shows that one's popup and exits. MinGW calls
// WinMain for both the console (Debug) and GUI (Release) builds.
#include <stdbool.h>
#include <string.h>
#include <windows.h>
#include <objbase.h>

#include "actions/stop.h"
#include "app/autostart.h"
#include "base/args.h"
#include "base/log.h"
#include "cli/cli.h"
#include "ui/single.h"
#include "ui/window.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show) {
  int argc = 0;
  char **argv = args_utf8(&argc);
  if (argc > 1 && strcmp(argv[1], ADMIN_STOP_FLAG) == 0) return stop_admin_main(argc, argv);
  if (cli_wants(argc, argv)) {
    cli_attach_parent_console();
    log_set_level(LOG_ERROR);
    return cli_main(argc, argv);
  }
  log_init();
  // Started with Windows: only the tray. Started by hand: the popup too.
  bool popup = !(argc > 1 && strcmp(argv[1], TRAY_FLAG) == 0);
  if (single_hand_over(popup)) return 0;
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);  // for ShellExecute
  log_info("app", "egret starting");
  if (!window_create(instance, popup)) return 1;
  MSG msg;
  while (GetMessageW(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }
  CoUninitialize();
  return (int)msg.wParam;
}
