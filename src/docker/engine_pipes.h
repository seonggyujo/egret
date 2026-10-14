// Which named pipes may serve a Docker engine, in the order to try them:
// DOCKER_HOST when it is an npipe:// address, then \\.\pipe\docker_engine,
// then \\.\pipe\dockerDesktopLinuxEngine. Only docker/ and the tests
// include this.
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

#define ENGINE_PIPES_MAX 3

// "npipe:////./pipe/docker_engine" or "npipe://./pipe/x" to "\\.\pipe\x".
// False for anything else (tcp://, unix://, empty).
bool engine_pipe_from_host(const char *host, wchar_t *out, size_t cap);

// Fills pipes (each MAX_PATH wide) and returns how many.
int engine_pipes(wchar_t pipes[ENGINE_PIPES_MAX][260]);
