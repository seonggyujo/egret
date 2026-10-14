// Stopping what holds a port: a container through Docker, a service through
// the service manager (elevated), any other process with TerminateProcess.
// Every call blocks, so the window runs them off its thread.
#pragma once

#include <stdbool.h>

#include "model/listener.h"

typedef enum {
  STOP_OK,         // it is gone
  STOP_GONE,       // it had already gone
  STOP_DENIED,     // needs administrator rights (or the UAC prompt was declined)
  STOP_BLOCKED,    // egret will not: system processes, WSL's forwarder
  STOP_NO_ENGINE,  // a container, but no Docker engine answered
  STOP_FAILED,
} StopResult;

// Whether stopping it needs administrator rights before even trying:
// services do.
bool stop_needs_admin(const Listener *l);

// Stops it without elevation. A process gets up to 3 s to end after
// TerminateProcess; a container up to 10 s before Docker kills it.
StopResult stop_listener(const Listener *l);

// Stops it through egret.exe started elevated (the UAC prompt): a service
// through the service manager (waiting up to 20 s), any other process with
// TerminateProcess. STOP_DENIED when the prompt is declined.
StopResult stop_elevated(const Listener *l);

// The elevated side: egret.exe --admin-stop <pid> [<service name>].
// Returns the StopResult as the process exit code.
#define ADMIN_STOP_FLAG "--admin-stop"
int stop_admin_main(int argc, char **argv);
