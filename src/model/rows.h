// Turning one look at the machine into rows, sorted by port, then PID.
//
// - One row per (port, PID). Its IPv4 and IPv6 sockets merge; lan is set
//   when any of them is a wildcard or a non-loopback address.
// - A port a container publishes, held by Docker's own forwarder
//   (com.docker.backend.exe, com.docker.proxy.exe, vpnkit.exe, dockerd.exe,
//   docker-proxy.exe, wslrelay.exe, gvproxy.exe), becomes that container.
// - wslrelay.exe holding a port no container publishes is KIND_WSL.
// - PID 0 and 4, and programs inside the Windows folder, are KIND_SYSTEM.
// - A process that could not be opened is KIND_SERVICE when it or its
//   parent is a service whose program is outside the Windows folder (its
//   folder is then that program's folder), else KIND_SYSTEM.
// - An opened process that is, or whose parent is, such a service is
//   KIND_SERVICE too. Every other opened process is KIND_PROCESS.
// - heron is set when heron-c.exe is among the ancestors, at most 16 up.
// - The folder is the working folder, but the program's folder when the
//   working folder is unknown or inside the Windows folder.
#pragma once

#include "model/listener.h"
#include "model/raw.h"

// Replaces out's rows.
void rows_build(const RawScan *raw, ListenerList *out);
