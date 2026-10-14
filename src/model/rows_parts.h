// What model/rows.c hands to model/rows_classify.c. Only model/rows*.c
// include this.
#pragma once

#include "model/listener.h"
#include "model/raw.h"

// Fills in a row that has its port, PID and address flags: names, path,
// folder, start time, heron, and its kind by the rules in model/rows.h.
void rows_classify(const RawScan *raw, Listener *row);
