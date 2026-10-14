// Flags and port numbers from the command line.
#include "cli/args.h"

#include <stdlib.h>
#include <string.h>

static bool flag(const char *arg, const char *short_name, const char *long_name) {
  return strcmp(arg, short_name) == 0 || strcmp(arg, long_name) == 0;
}

// A port number 1..65535 with nothing after it.
static bool port_of(const char *arg, uint16_t *out) {
  if (arg[0] < '0' || arg[0] > '9' || strlen(arg) > 5) return false;
  char *end;
  long v = strtol(arg, &end, 10);
  if (*end || v < 1 || v > 65535) return false;
  *out = (uint16_t)v;
  return true;
}

static void add_port(CliArgs *out, uint16_t port) {
  for (int i = 0; i < out->port_count; i++) {
    if (out->ports[i] == port) return;
  }
  if (out->port_count < CLI_MAX_PORTS) out->ports[out->port_count++] = port;
}

bool cli_parse(int argc, char **argv, CliArgs *out) {
  memset(out, 0, sizeof *out);
  for (int i = 1; i < argc; i++) {
    const char *a = argv[i];
    uint16_t port;
    if (flag(a, "-l", "--list")) out->list = true;
    else if (flag(a, "-a", "--all")) out->all = true;
    else if (flag(a, "-y", "--yes")) out->yes = true;
    else if (flag(a, "-h", "--help") || strcmp(a, "/?") == 0) out->help = true;
    else if (flag(a, "-V", "--version")) out->version = true;
    else if (port_of(a, &port)) add_port(out, port);
    else {
      out->bad = a;
      return false;
    }
  }
  if (out->list && out->port_count) {
    out->bad = NULL;  // nothing wrong with any one argument
    return false;
  }
  return true;
}
