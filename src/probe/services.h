// Running Win32 services: the PID each runs in and the program its command
// line starts. Only services in the processes that hold a socket, or in
// their parents, are looked at, so a scan stays cheap.
#pragma once

#include <stdbool.h>

#include "model/raw.h"

// Fills raw->services. Needs raw->sockets and raw->procs. False when the
// service manager could not be asked.
bool probe_services(RawScan *raw);
