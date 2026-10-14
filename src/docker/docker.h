// Docker containers through the Engine API, spoken as HTTP/1.1 over the
// named pipe an engine serves on Windows: DOCKER_HOST when it is an
// npipe:// address, else \\.\pipe\docker_engine, then
// \\.\pipe\dockerDesktopLinuxEngine. When no engine answers nothing is
// shown and nothing is logged above debug.
#pragma once

#include <stdbool.h>

#include "model/raw.h"

// Fills raw->containers with the running containers and the TCP ports they
// publish. False, with none, when no engine answered. Waits about a second
// at most.
bool docker_list(RawScan *raw);

typedef enum { DOCKER_OK, DOCKER_NO_ENGINE, DOCKER_GONE, DOCKER_FAILED } DockerResult;

// Stops a container (POST /containers/{id}/stop), giving it up to 10 s
// before the engine kills it. Blocks until the engine answers.
DockerResult docker_stop(const char *id);
