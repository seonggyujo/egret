// The current time in FILETIME units (100 ns since 1601, UTC), the unit
// process start times come in.
#pragma once

#include <stdint.h>

uint64_t clock_filetime_now(void);
