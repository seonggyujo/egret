// The shortcut's text form: parsing "Ctrl+Alt+P" and writing it back.
#include "app/hotkey.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "base/names.h"

static const struct {
  const char *name;
  unsigned mod;
} MODS[] = {{"Ctrl", MOD_CONTROL}, {"Control", MOD_CONTROL}, {"Alt", MOD_ALT}, {"Shift", MOD_SHIFT},
            {"Win", MOD_WIN},      {"Windows", MOD_WIN}};

static const struct {
  const char *name;
  unsigned vk;
} NAMED[] = {{"Space", VK_SPACE},   {"Home", VK_HOME},       {"End", VK_END},     {"Insert", VK_INSERT},
             {"Delete", VK_DELETE}, {"PageUp", VK_PRIOR},    {"PageDown", VK_NEXT}, {"Up", VK_UP},
             {"Down", VK_DOWN},     {"Left", VK_LEFT},       {"Right", VK_RIGHT}};

// The key a name stands for, 0 for none.
static unsigned key_of(const char *name) {
  size_t n = strlen(name);
  if (n == 1 && ((name[0] >= 'A' && name[0] <= 'Z') || (name[0] >= 'a' && name[0] <= 'z'))) {
    return (unsigned)(name[0] & ~0x20);
  }
  if (n == 1 && name[0] >= '0' && name[0] <= '9') return (unsigned)name[0];
  if ((name[0] == 'F' || name[0] == 'f') && n >= 2 && n <= 3) {
    int f = 0;
    if (sscanf(name + 1, "%d", &f) == 1 && f >= 1 && f <= 24) return VK_F1 + (unsigned)f - 1;
  }
  for (size_t i = 0; i < sizeof NAMED / sizeof NAMED[0]; i++) {
    if (name_equal(name, NAMED[i].name)) return NAMED[i].vk;
  }
  return 0;
}

static unsigned mod_of(const char *name) {
  for (size_t i = 0; i < sizeof MODS / sizeof MODS[0]; i++) {
    if (name_equal(name, MODS[i].name)) return MODS[i].mod;
  }
  return 0;
}

bool hotkey_parse(const char *text, unsigned *mods, unsigned *vk) {
  *mods = *vk = 0;
  while (text && *text) {
    const char *plus = strchr(text, '+');
    size_t len = plus ? (size_t)(plus - text) : strlen(text);
    char part[16];
    while (len && *text == ' ') text++, len--;
    while (len && text[len - 1] == ' ') len--;
    if (len == 0 || len >= sizeof part) return false;
    memcpy(part, text, len);
    part[len] = 0;
    unsigned m = mod_of(part), k = m ? 0 : key_of(part);
    if (!m && (!k || *vk)) return false;  // unknown, or a second key
    *mods |= m;
    if (k) *vk = k;
    text = plus ? plus + 1 : NULL;
  }
  return hotkey_valid(*mods, *vk);
}

static void key_name(unsigned vk, char *out, size_t cap) {
  if ((vk >= 'A' && vk <= 'Z') || (vk >= '0' && vk <= '9')) snprintf(out, cap, "%c", (char)vk);
  else if (vk >= VK_F1 && vk <= VK_F24) snprintf(out, cap, "F%u", vk - VK_F1 + 1);
  else snprintf(out, cap, "?");
  for (size_t i = 0; i < sizeof NAMED / sizeof NAMED[0]; i++) {
    if (NAMED[i].vk == vk) snprintf(out, cap, "%s", NAMED[i].name);
  }
}

void hotkey_format(unsigned mods, unsigned vk, char *out, size_t cap) {
  char key[16];
  key_name(vk, key, sizeof key);
  snprintf(out, cap, "%s%s%s%s%s", mods & MOD_CONTROL ? "Ctrl+" : "", mods & MOD_ALT ? "Alt+" : "",
           mods & MOD_SHIFT ? "Shift+" : "", mods & MOD_WIN ? "Win+" : "", key);
}

bool hotkey_valid(unsigned mods, unsigned vk) {
  if (!(mods & (MOD_CONTROL | MOD_ALT | MOD_WIN))) return false;
  char name[16];
  key_name(vk, name, sizeof name);
  return strcmp(name, "?") != 0;
}
