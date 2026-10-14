// The command line:
//   egret --list [--all]       the table of listening ports
//   egret <port>... [-y]       stops what holds those ports, asking first unless -y
//   egret --help, egret --version
// Output is text in the language of the settings: UTF-16 to a console,
// UTF-8 bytes to a pipe or a file. Exit codes: 0 done, 1 some port could not
// be stopped, 2 bad arguments or a port nobody listens on, 3 cancelled.
#pragma once

#include <stdbool.h>

// Whether these arguments ask for the command line rather than the tray
// app: anything except no arguments, --tray and --admin-stop.
bool cli_wants(int argc, char **argv);

// argv[0] is the program. Returns the exit code.
int cli_main(int argc, char **argv);

// egret.exe is a window program: started from a terminal with command line
// arguments it borrows the terminal's console for its output, unless its
// output already goes to a pipe or a file.
void cli_attach_parent_console(void);
