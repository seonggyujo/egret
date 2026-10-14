// Growable byte buffer, used for HTTP replies from the Docker engine and
// text built in pieces. A zeroed Buf is a valid empty buffer.
#pragma once

#include <stddef.h>

typedef struct {
  char *data;
  size_t len, cap;
} Buf;

void buf_append(Buf *b, const void *data, size_t n);
void buf_append_str(Buf *b, const char *s);

void buf_clear(Buf *b);
void buf_free(Buf *b);
