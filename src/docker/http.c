// Parsing a whole HTTP/1.1 response: status line, headers, body.
#include "docker/http.h"

#include <stdlib.h>
#include <string.h>

#include "base/names.h"

typedef struct {
  const char *at, *end;
} Cursor;

// The next line without its CRLF, or NULL when the line is not complete.
static const char *next_line(Cursor *c, size_t *len) {
  for (const char *p = c->at; p + 1 < c->end; p++) {
    if (p[0] == '\r' && p[1] == '\n') {
      const char *line = c->at;
      *len = (size_t)(p - line);
      c->at = p + 2;
      return line;
    }
  }
  return NULL;
}

// The value of a header line when its name is name, NUL-terminated in out.
static bool header_value(const char *line, size_t len, const char *name, char *out, size_t cap) {
  size_t n = strlen(name);
  if (len <= n || line[n] != ':' || !text_starts_with(line, name)) return false;
  const char *v = line + n + 1;
  while (v < line + len && (*v == ' ' || *v == '\t')) v++;
  size_t vn = (size_t)(line + len - v);
  if (vn >= cap) vn = cap - 1;
  memcpy(out, v, vn);
  out[vn] = 0;
  return true;
}

static HttpState read_chunks(Cursor *c, bool at_eof, Buf *body) {
  for (;;) {
    size_t len;
    const char *line = next_line(c, &len);
    if (!line) return at_eof ? HTTP_BAD : HTTP_INCOMPLETE;
    char *stop;
    unsigned long size = strtoul(line, &stop, 16);
    if (stop == line) return HTTP_BAD;
    if (size == 0) {  // trailers up to an empty line
      while ((line = next_line(c, &len)) && len) {}
      return line || at_eof ? HTTP_DONE : HTTP_INCOMPLETE;
    }
    if ((size_t)(c->end - c->at) < size + 2) return at_eof ? HTTP_BAD : HTTP_INCOMPLETE;
    buf_append(body, c->at, size);
    c->at += size + 2;
  }
}

static HttpState read_body(Cursor *c, bool at_eof, bool chunked, long length, HttpResponse *out) {
  if (out->status / 100 == 1 || out->status == 204 || out->status == 304) return HTTP_DONE;
  HttpState st;
  if (chunked) {
    st = read_chunks(c, at_eof, &out->body);
  } else if (length >= 0) {
    if (c->end - c->at < length) return at_eof ? HTTP_BAD : HTTP_INCOMPLETE;
    buf_append(&out->body, c->at, (size_t)length);
    st = HTTP_DONE;
  } else {
    if (!at_eof) return HTTP_INCOMPLETE;
    buf_append(&out->body, c->at, (size_t)(c->end - c->at));
    st = HTTP_DONE;
  }
  if (st == HTTP_DONE) {
    buf_append(&out->body, "", 1);
    out->body.len--;
  }
  return st;
}

HttpState http_parse(const char *data, size_t len, bool at_eof, HttpResponse *out) {
  buf_clear(&out->body);
  out->status = 0;
  Cursor c = {data, data + len};
  size_t n;
  const char *line = next_line(&c, &n);
  if (!line) return at_eof ? HTTP_BAD : HTTP_INCOMPLETE;
  if (n < 12 || strncmp(line, "HTTP/1.", 7) != 0) return HTTP_BAD;
  out->status = atoi(line + 9);
  bool chunked = false;
  long length = -1;
  char value[64];
  while ((line = next_line(&c, &n)) && n) {
    if (header_value(line, n, "content-length", value, sizeof value)) length = atol(value);
    if (header_value(line, n, "transfer-encoding", value, sizeof value)) chunked = text_contains(value, "chunked", 7);
  }
  if (!line) return at_eof ? HTTP_BAD : HTTP_INCOMPLETE;
  return read_body(&c, at_eof, chunked, length, out);
}

void http_response_free(HttpResponse *r) { buf_free(&r->body); }
