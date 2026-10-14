// The pipes to try for a Docker engine.
#include "docker/engine_pipes.h"

#include <stdlib.h>
#include <string.h>

#include "base/names.h"
#include "base/utf.h"

bool engine_pipe_from_host(const char *host, wchar_t *out, size_t cap) {
  if (!host || !text_starts_with(host, "npipe://")) return false;
  const char *p = host + 8;
  while (*p == '/') p++;  // npipe:////./pipe/x has two more
  if (!text_starts_with(p, "./pipe/") || !p[7]) return false;
  wchar_t *name = utf8_to_wide(p + 7, -1);
  bool ok = wcslen(name) + 10 < cap;
  if (ok) {
    wcscpy(out, L"\\\\.\\pipe\\");
    wcscat(out, name);
    for (wchar_t *c = out; *c; c++) {
      if (*c == L'/') *c = L'\\';
    }
  }
  free(name);
  return ok;
}

int engine_pipes(wchar_t pipes[ENGINE_PIPES_MAX][260]) {
  int n = 0;
  if (engine_pipe_from_host(getenv("DOCKER_HOST"), pipes[n], 260)) n++;
  static const wchar_t *const DEFAULTS[] = {L"\\\\.\\pipe\\docker_engine", L"\\\\.\\pipe\\dockerDesktopLinuxEngine"};
  for (size_t i = 0; i < sizeof DEFAULTS / sizeof DEFAULTS[0]; i++) {
    if (n == 0 || wcscmp(pipes[0], DEFAULTS[i]) != 0) wcscpy(pipes[n++], DEFAULTS[i]);
  }
  return n;
}
