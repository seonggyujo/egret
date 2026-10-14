// UTF-8 text to the clipboard as UTF-16.
#include "ui/clipboard.h"

#include <string.h>

void clipboard_set(HWND owner, const char *utf8) {
  int n = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
  HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, (size_t)n * sizeof(wchar_t));
  if (!mem) return;
  MultiByteToWideChar(CP_UTF8, 0, utf8, -1, GlobalLock(mem), n);
  GlobalUnlock(mem);
  if (!OpenClipboard(owner)) {
    GlobalFree(mem);
    return;
  }
  EmptyClipboard();
  if (!SetClipboardData(CF_UNICODETEXT, mem)) GlobalFree(mem);
  CloseClipboard();
}
