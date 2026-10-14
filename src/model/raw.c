// Freeing the arrays of one look at the machine.
#include "model/raw.h"

#include <stdlib.h>
#include <string.h>

void raw_scan_free(RawScan *raw) {
  free(raw->sockets);
  free(raw->procs);
  free(raw->infos);
  free(raw->services);
  free(raw->containers);
  memset(raw, 0, sizeof *raw);
}
