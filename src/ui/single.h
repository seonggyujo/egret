// One egret per user: a second launch asks the running one to show its
// popup and exits. Debug builds use their own names, so a dev run never
// talks to an installed egret.
#pragma once

#include <stdbool.h>

#ifdef EGRET_DEV
#define SINGLE_CLASS L"egret.popup.dev"
#else
#define SINGLE_CLASS L"egret.popup"
#endif

// Before the window exists. True when another egret runs; it was asked to
// show its popup when show is true. The caller then exits.
bool single_hand_over(bool show);
