// Reading the Engine API's container list (GET /containers/json) into
// RawContainer entries. Pure, so it is tested on its own.
#pragma once

#include <stddef.h>

#include "model/raw.h"

// How many containers, -1 when the text is not such a list. *out is
// malloc'd (NULL when none).
int docker_parse_containers(const char *json, size_t len, RawContainer **out);
