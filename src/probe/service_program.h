// The program a service's command line starts, as a full path. Pure text
// work, so it is tested on its own.
#pragma once

#include <stddef.h>

// command is the service's command line with environment variables
// already expanded: "\"C:\Program Files\x\x.exe\" run", "C:\Program
// Files\y\y.exe -k" (unquoted, spaces and all), "\SystemRoot\..." or
// "system32\x.exe" (relative to the Windows folder), "\??\C:\...".
void service_program(const char *command, const char *windows_dir, char *out, size_t cap);
