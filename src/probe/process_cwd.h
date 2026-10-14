// A process's current folder, read from its PEB: the process parameters'
// CurrentDirectory. Handles 64-bit and 32-bit (WOW64) processes. Only
// probe/ includes this.
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <windows.h>

// h needs PROCESS_QUERY_INFORMATION and PROCESS_VM_READ. Writes UTF-8
// without a trailing backslash ("C:\" keeps its own).
bool process_cwd_read(HANDLE h, char *out, size_t cap);
