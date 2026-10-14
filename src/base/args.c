// The command line through CommandLineToArgvW, converted to UTF-8.
#include "base/args.h"

#include <windows.h>
// After windows.h, which defines what shellapi.h uses.
#include <shellapi.h>

#include "base/mem.h"
#include "base/utf.h"

char **args_utf8(int *argc) {
  int n = 0;
  wchar_t **wide = CommandLineToArgvW(GetCommandLineW(), &n);
  char **argv = xcalloc((size_t)n + 1, sizeof *argv);
  for (int i = 0; i < n; i++) argv[i] = wide_to_utf8(wide[i], -1, NULL);
  LocalFree(wide);
  *argc = n;
  return argv;
}
