// Reading the command line's arguments. Pure, so it is tested on its own.
// Only cli/ and the tests include this.
#pragma once

#include <stdbool.h>
#include <stdint.h>

#define CLI_MAX_PORTS 64

typedef struct {
  bool list, all, yes, help, version;
  uint16_t ports[CLI_MAX_PORTS];  // each once, in the order given
  int port_count;
  const char *bad;  // the argument that was not understood
} CliArgs;

// argv[0] is the program. False, with bad set, for an unknown flag or a
// number outside 1..65535; false with bad NULL for --list mixed with ports.
bool cli_parse(int argc, char **argv, CliArgs *out);
