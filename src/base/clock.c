// The current time from the system clock.
#include "base/clock.h"

#include <windows.h>

uint64_t clock_filetime_now(void) {
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  return ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
}
