// The shortcut text, the hidden programs list and PATH list edits.
#include <windows.h>

#include "app/hidden.h"
#include "app/hotkey.h"
#include "app/path_list.h"
#include "check.h"

void test_hotkey(void) {
  unsigned mods, vk;
  char text[32];
  CHECK(hotkey_parse("Ctrl+Alt+P", &mods, &vk) && mods == (MOD_CONTROL | MOD_ALT) && vk == 'P');
  CHECK(hotkey_parse(" ctrl + shift + f12 ", &mods, &vk) && mods == (MOD_CONTROL | MOD_SHIFT) && vk == VK_F12);
  CHECK(hotkey_parse("Win+Alt+Space", &mods, &vk) && vk == VK_SPACE);
  CHECK(!hotkey_parse("Shift+P", &mods, &vk));    // Shift alone is not enough
  CHECK(!hotkey_parse("Ctrl+P+Q", &mods, &vk));   // two keys
  CHECK(!hotkey_parse("Ctrl+Alt", &mods, &vk));   // no key
  CHECK(!hotkey_parse("Ctrl+Hyper", &mods, &vk));
  CHECK(!hotkey_parse("Ctrl+F25", &mods, &vk));
  hotkey_format(MOD_WIN | MOD_SHIFT | MOD_CONTROL, VK_NEXT, text, sizeof text);
  CHECK_STR(text, "Ctrl+Shift+Win+PageDown");
  hotkey_format(MOD_CONTROL | MOD_ALT, '7', text, sizeof text);
  CHECK_STR(text, "Ctrl+Alt+7");
  CHECK(hotkey_valid(MOD_ALT, 'A') && !hotkey_valid(MOD_ALT, VK_LSHIFT));
}

void test_hidden_list(void) {
  HiddenList h;
  char joined[256];
  hidden_parse(&h, " Discord.exe;;RiotClientServices.exe; discord.EXE ;");
  CHECK(h.count == 2);
  CHECK(hidden_contains(&h, "DISCORD.exe") && !hidden_contains(&h, "node.exe"));
  hidden_join(&h, joined, sizeof joined);
  CHECK_STR(joined, "Discord.exe;RiotClientServices.exe");
  const char *names[4];
  CHECK(hidden_pointers(&h, names, 4) == 2);
  CHECK_STR(names[1], "RiotClientServices.exe");
  hidden_parse(&h, "");
  CHECK(h.count == 0);
}

void test_path_list(void) {
  wchar_t out[256];
  CHECK(path_list_has(L"C:\\a;C:\\Tools\\Egret\\;C:\\b", L"c:\\tools\\egret"));
  CHECK(!path_list_has(L"C:\\a;C:\\Tools\\EgretX", L"C:\\Tools\\Egret"));
  CHECK(path_list_with(L"C:\\a;;C:\\b;", L"C:\\e", true, out, 256) && wcscmp(out, L"C:\\a;C:\\b;C:\\e") == 0);
  CHECK(path_list_with(L"", L"C:\\e", true, out, 256) && wcscmp(out, L"C:\\e") == 0);
  CHECK(path_list_with(L"C:\\e\\;C:\\a;c:\\E", L"C:\\e", false, out, 256) && wcscmp(out, L"C:\\a") == 0);
  CHECK(path_list_with(L"C:\\a;C:\\e", L"C:\\e", true, out, 256) && wcscmp(out, L"C:\\a;C:\\e") == 0);
  CHECK(!path_list_with(L"C:\\aaaaaaaa", L"C:\\e", true, out, 8));
}
