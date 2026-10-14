// The machine's listening TCP sockets, IPv4 and IPv6, with the PID that
// owns each (GetExtendedTcpTable, TCP_TABLE_OWNER_PID_LISTENER).
#pragma once

#include <stdbool.h>

#include "model/raw.h"

// Fills raw->sockets. False when neither table could be read.
bool probe_sockets(RawScan *raw);
