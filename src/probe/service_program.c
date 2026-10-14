// Cutting the program path out of a service command line.
#include "probe/service_program.h"

#include <stdio.h>
#include <string.h>

#include "base/names.h"

// Where the program part of an unquoted command ends: after ".exe" when
// there is one (paths with spaces are often left unquoted), else at the
// first space.
static size_t unquoted_end(const char *s) {
  for (const char *p = s; *p; p++) {
    if (text_starts_with(p, ".exe") && (p[4] == 0 || p[4] == ' ' || p[4] == '\t')) return (size_t)(p - s) + 4;
  }
  const char *space = strpbrk(s, " \t");
  return space ? (size_t)(space - s) : strlen(s);
}

void service_program(const char *command, const char *windows_dir, char *out, size_t cap) {
  const char *s = command;
  while (*s == ' ' || *s == '\t') s++;
  size_t n;
  if (*s == '"') {
    const char *close = strchr(++s, '"');
    n = close ? (size_t)(close - s) : strlen(s);
  } else {
    n = unquoted_end(s);
  }
  char program[2048];
  if (n >= sizeof program) n = sizeof program - 1;
  memcpy(program, s, n);
  program[n] = 0;
  const char *p = program;
  if (text_starts_with(p, "\\??\\")) p += 4;
  if (text_starts_with(p, "\\SystemRoot\\")) snprintf(out, cap, "%s\\%s", windows_dir, p + 12);
  else if ((p[0] && p[1] == ':') || (p[0] == '\\' && p[1] == '\\')) text_copy(out, cap, p);
  else if (p[0]) snprintf(out, cap, "%s\\%s", windows_dir, p);  // relative to the Windows folder
  else out[0] = 0;
}
