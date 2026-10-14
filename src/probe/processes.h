// The system's process snapshot: every PID, its parent and its image file
// name. Readable for every process, even ones that cannot be opened.
#pragma once

#include <stdbool.h>

#include "model/raw.h"

// Fills raw->procs. False when the snapshot could not be taken.
bool probe_processes(RawScan *raw);
