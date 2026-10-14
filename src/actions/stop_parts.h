// The two ways actions/stop.c ends something on this machine. Only
// actions/ includes this.
#pragma once

#include <stdint.h>

#include "actions/stop.h"

// Terminates pid. When exe or started are known they must still match,
// so a PID reused by another program is left alone (STOP_GONE).
StopResult stop_process(uint32_t pid, const char *exe, uint64_t started);

// Asks the service manager to stop the service and waits up to 20 s.
StopResult stop_service(const char *name);
