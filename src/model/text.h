// Texts the list and the command line both show.
#pragma once

#include <stddef.h>
#include <stdint.h>

// How long since started, both in FILETIME units: "41s", "12m", "2h 13m",
// "3d 4h". "-" when started is 0 or later than now.
void text_uptime(char *out, size_t cap, uint64_t started, uint64_t now);

// path with the home folder replaced by "~" ("~\IdeaProjects\egret"), when
// it is inside it (ASCII case-insensitive); otherwise path as it is.
void text_short_path(char *out, size_t cap, const char *path, const char *home);
