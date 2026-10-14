// egret's folder in the user's PATH (HKCU\Environment), so "egret" works in
// terminals opened after the change. Setting it tells running programs
// with WM_SETTINGCHANGE "Environment".
#pragma once

#include <stdbool.h>

bool user_path_enabled(void);
bool user_path_set(bool on);
