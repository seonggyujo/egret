// The two jobs cli/cli.c hands out. Only cli/ includes this.
#pragma once

#include <stdbool.h>

#include "cli/args.h"

// Prints the --list table. Returns the exit code.
int cli_list(bool all);

// Stops what holds a->ports, asking first unless a->yes. Returns the exit
// code.
int cli_stop(const CliArgs *a);
