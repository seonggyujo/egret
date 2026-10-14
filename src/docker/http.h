// Reading one HTTP/1.1 response from the bytes received so far: the status,
// the headers, and the body by Content-Length, chunked, or up to the end of
// the stream. Pure, so it is tested on its own.
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "base/buf.h"

typedef enum { HTTP_INCOMPLETE, HTTP_DONE, HTTP_BAD } HttpState;

typedef struct {
  int status;
  Buf body;  // NUL-terminated past len when there is a body
} HttpResponse;

// at_eof: the server closed the stream, so a body without a length ends
// here. Parses from the start each time; out->body is replaced.
HttpState http_parse(const char *data, size_t len, bool at_eof, HttpResponse *out);
void http_response_free(HttpResponse *r);
