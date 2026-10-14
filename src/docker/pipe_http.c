// HTTP over a named pipe with overlapped I/O, so a hung engine cannot hold
// the scan past its deadline.
#include "docker/pipe_http.h"

#include <stdbool.h>
#include <windows.h>

#include "base/log.h"

#define BUSY_WAIT_MS 300

static HANDLE open_pipe(const wchar_t *pipe, PipeResult *why) {
  for (int tries = 0; tries < 2; tries++) {
    HANDLE h = CreateFileW(pipe, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (h != INVALID_HANDLE_VALUE) return h;
    DWORD err = GetLastError();
    if (err == ERROR_PIPE_BUSY && tries == 0 && WaitNamedPipeW(pipe, BUSY_WAIT_MS)) continue;
    *why = err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND ? PIPE_NO_SERVER : PIPE_FAILED;
    log_debug("docker", "%ls: cannot open (%lu)", pipe, err);
    return NULL;
  }
  *why = PIPE_FAILED;
  return NULL;
}

static DWORD ms_left(ULONGLONG deadline) {
  ULONGLONG now = GetTickCount64();
  return now >= deadline ? 0 : (DWORD)(deadline - now);
}

// Runs one overlapped read or write to its end or the deadline. *done gets
// the bytes moved; false on failure, timeout, or (for a read) a closed pipe
// with *eof set.
static bool finish(HANDLE h, OVERLAPPED *ov, BOOL started, ULONGLONG deadline, DWORD *done, bool *eof) {
  if (!started && GetLastError() != ERROR_IO_PENDING) {
    DWORD err = GetLastError();
    if (err == ERROR_BROKEN_PIPE) *eof = true;
    return false;
  }
  if (WaitForSingleObject(ov->hEvent, ms_left(deadline)) != WAIT_OBJECT_0) {
    CancelIoEx(h, ov);
    GetOverlappedResult(h, ov, done, TRUE);
    log_debug("docker", "pipe timed out");
    return false;
  }
  if (GetOverlappedResult(h, ov, done, FALSE)) return true;
  DWORD err = GetLastError();
  if (err == ERROR_BROKEN_PIPE) *eof = true;
  return err == ERROR_MORE_DATA;
}

static PipeResult read_response(HANDLE h, OVERLAPPED *ov, ULONGLONG deadline, HttpResponse *out) {
  Buf got = {0};
  char chunk[16384];
  HttpState st = HTTP_INCOMPLETE;
  for (;;) {
    DWORD n = 0;
    bool eof = false;
    ResetEvent(ov->hEvent);
    bool ok = finish(h, ov, ReadFile(h, chunk, sizeof chunk, NULL, ov), deadline, &n, &eof);
    if (ok) buf_append(&got, chunk, n);
    if (ok || eof) st = http_parse(got.data ? got.data : "", got.len, eof, out);
    if (!ok || st != HTTP_INCOMPLETE) break;
  }
  buf_free(&got);
  return st == HTTP_DONE ? PIPE_OK : PIPE_FAILED;
}

PipeResult pipe_http_exchange(const wchar_t *pipe, const char *request, size_t len, int timeout_ms,
                              HttpResponse *out) {
  PipeResult result = PIPE_FAILED;
  HANDLE h = open_pipe(pipe, &result);
  if (!h) return result;
  ULONGLONG deadline = GetTickCount64() + (ULONGLONG)timeout_ms;
  OVERLAPPED ov = {0};
  ov.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
  DWORD n = 0;
  bool eof = false;
  result = PIPE_FAILED;
  if (ov.hEvent && finish(h, &ov, WriteFile(h, request, (DWORD)len, NULL, &ov), deadline, &n, &eof) && n == len) {
    result = read_response(h, &ov, deadline, out);
  }
  if (ov.hEvent) CloseHandle(ov.hEvent);
  CloseHandle(h);
  return result;
}
