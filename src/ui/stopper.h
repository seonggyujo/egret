// Stopping a row off the window's thread (a container can take 10 s, a
// service 20 s, the UAC prompt as long as the user likes). One at a time;
// the result comes back as WM_APP_STOPPED and shows in the status bar, or
// in a tray notification when the popup has closed meanwhile.
#pragma once

#include <stdbool.h>

#include "model/listener.h"
#include "ui/app.h"

typedef struct StopJob {
  HWND hwnd;
  Listener row;
  bool admin;
  int result;  // a StopResult
} StopJob;

// Starts stopping row (copied). False when another stop is still running.
bool stopper_start(App *a, const Listener *row, bool admin);
// WM_APP_STOPPED: shows the outcome, rescans, frees the job.
void stopper_done(App *a, StopJob *job);
