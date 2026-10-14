// Editing a ';'-separated folder list such as PATH: entries compare
// case-insensitively and ignore a trailing backslash; empty entries stay
// out. Pure, so it is tested on its own. Only app/ and the tests include
// this.
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

bool path_list_has(const wchar_t *list, const wchar_t *dir);

// out gets list with dir added at the end (once) or every match of it
// removed. False when out is too small.
bool path_list_with(const wchar_t *list, const wchar_t *dir, bool present, wchar_t *out, size_t cap);
