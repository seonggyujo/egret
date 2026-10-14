// The command line's entry: which job the arguments ask for, and borrowing
// the terminal's console when egret.exe (a window program) runs one.
#include "cli/cli.h"

#include <string.h>
#include <windows.h>

#include "actions/stop.h"
#include "app/autostart.h"
#include "app/strings.h"
#include "app/version.h"
#include "cli/args.h"
#include "cli/cli_parts.h"
#include "cli/out.h"

#define EXIT_USAGE 2

bool cli_wants(int argc, char **argv) {
  return argc > 1 && strcmp(argv[1], TRAY_FLAG) != 0 && strcmp(argv[1], ADMIN_STOP_FLAG) != 0;
}

int cli_main(int argc, char **argv) {
  CliArgs a;
  if (!cli_parse(argc, argv, &a)) {
    if (a.bad) {
      char msg[300];
      format_cli_not_port(msg, sizeof msg, a.bad);
      err_line(msg);
    }
    err_line(strings()->cli_usage);
    return EXIT_USAGE;
  }
  if (a.help) {
    out_text(strings()->cli_usage);
    return 0;
  }
  if (a.version) {
    out_line("egret " EGRET_VERSION);
    return 0;
  }
  if (a.list) return cli_list(a.all);
  if (a.port_count) return cli_stop(&a);
  err_line(strings()->cli_usage);
  return EXIT_USAGE;
}

static bool usable(HANDLE h) { return h && h != INVALID_HANDLE_VALUE && GetFileType(h) != FILE_TYPE_UNKNOWN; }

static HANDLE open_console(const wchar_t *name) {
  return CreateFileW(name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0,
                     NULL);
}

void cli_attach_parent_console(void) {
  if (usable(GetStdHandle(STD_OUTPUT_HANDLE))) return;
  if (!AttachConsole(ATTACH_PARENT_PROCESS)) return;
  HANDLE out = open_console(L"CONOUT$"), in = open_console(L"CONIN$");
  if (out != INVALID_HANDLE_VALUE) {
    SetStdHandle(STD_OUTPUT_HANDLE, out);
    if (!usable(GetStdHandle(STD_ERROR_HANDLE))) SetStdHandle(STD_ERROR_HANDLE, out);
  }
  if (in != INVALID_HANDLE_VALUE && !usable(GetStdHandle(STD_INPUT_HANDLE))) SetStdHandle(STD_INPUT_HANDLE, in);
}
