// One full look at the machine turned into rows: sockets, the process
// snapshot, the listening processes' details, services, Docker containers,
// then model/rows.h. Blocks for up to about a second when Docker is slow.
// Shares nothing, so any thread may call it.
#pragma once

#include "model/listener.h"

// Replaces out's rows (a zeroed list is fine).
void scan_run(ListenerList *out);
