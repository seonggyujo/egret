// Reading CurrentDirectory.DosPath from another process's PEB. The offsets
// are those of the PEB and RTL_USER_PROCESS_PARAMETERS on x64 and, for a
// 32-bit process under WOW64, of their 32-bit forms.
#include "probe/process_cwd.h"

#include <stdint.h>
#include <winternl.h>

#define PEB64_PARAMS 0x20
#define PARAMS64_CWD 0x38
#define PEB32_PARAMS 0x10
#define PARAMS32_CWD 0x24
#define MAX_CWD_BYTES 0x8000

static bool read_mem(HANDLE h, uintptr_t at, void *out, size_t n) {
  SIZE_T got = 0;
  return at && ReadProcessMemory(h, (LPCVOID)at, out, n, &got) && got == n;
}

// The UTF-16 text at buffer, len bytes long, as UTF-8 without a trailing
// backslash.
static bool read_text(HANDLE h, uintptr_t buffer, unsigned len, char *out, size_t cap) {
  if (len == 0 || len > MAX_CWD_BYTES || len % 2) return false;
  wchar_t text[MAX_CWD_BYTES / 2 + 1];
  if (!read_mem(h, buffer, text, len)) return false;
  size_t chars = len / 2;
  if (chars > 3 && text[chars - 1] == L'\\') chars--;
  text[chars] = 0;
  return WideCharToMultiByte(CP_UTF8, 0, text, -1, out, (int)cap, NULL, NULL) > 0;
}

static bool read_64(HANDLE h, char *out, size_t cap) {
  PROCESS_BASIC_INFORMATION pbi;
  ULONG got = 0;
  if (NtQueryInformationProcess(h, ProcessBasicInformation, &pbi, sizeof pbi, &got) != 0) return false;
  uint64_t params = 0;
  if (!read_mem(h, (uintptr_t)pbi.PebBaseAddress + PEB64_PARAMS, &params, sizeof params)) return false;
  struct {
    uint16_t len, max;
    uint32_t pad;
    uint64_t buffer;
  } cwd;
  if (!read_mem(h, (uintptr_t)params + PARAMS64_CWD, &cwd, sizeof cwd)) return false;
  return read_text(h, (uintptr_t)cwd.buffer, cwd.len, out, cap);
}

static bool read_32(HANDLE h, uintptr_t peb32, char *out, size_t cap) {
  uint32_t params = 0;
  if (!read_mem(h, peb32 + PEB32_PARAMS, &params, sizeof params)) return false;
  struct {
    uint16_t len, max;
    uint32_t buffer;
  } cwd;
  if (!read_mem(h, (uintptr_t)params + PARAMS32_CWD, &cwd, sizeof cwd)) return false;
  return read_text(h, (uintptr_t)cwd.buffer, cwd.len, out, cap);
}

bool process_cwd_read(HANDLE h, char *out, size_t cap) {
  ULONG_PTR peb32 = 0;
  ULONG got = 0;
  NTSTATUS st = NtQueryInformationProcess(h, ProcessWow64Information, &peb32, sizeof peb32, &got);
  if (st == 0 && peb32) return read_32(h, peb32, out, cap);
  return read_64(h, out, cap);
}
