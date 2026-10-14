// What one process tells without administrator rights: its program, its
// start time and its current folder.
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "model/raw.h"

// Fills out for pid. Whatever cannot be read stays empty.
void probe_process_info(uint32_t pid, RawProcInfo *out);

// The Windows folder ("C:\Windows"), UTF-8.
void probe_windows_dir(char *out, size_t cap);
