// One HTTP request and its response over a named pipe, with a deadline:
// connect, write, then read until the response is complete or the server
// closes the pipe. Only docker/ includes this.
#pragma once

#include <stddef.h>
#include <wchar.h>

#include "docker/http.h"

typedef enum { PIPE_OK, PIPE_NO_SERVER, PIPE_FAILED } PipeResult;

// PIPE_NO_SERVER when nothing serves the pipe (the engine is not running).
PipeResult pipe_http_exchange(const wchar_t *pipe, const char *request, size_t len, int timeout_ms,
                              HttpResponse *out);
