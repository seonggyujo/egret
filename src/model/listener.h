// One row of the list: a listening TCP port and what holds it. The scan
// makes them (model/rows.h), the window and the command line show them.
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "model/raw.h"

typedef enum {
  KIND_PROCESS,    // an ordinary program; dev servers land here
  KIND_SERVICE,    // a Windows service whose program is outside the Windows folder (a local DB)
  KIND_CONTAINER,  // a port a Docker container publishes
  KIND_WSL,        // wslrelay.exe forwarding a port out of WSL
  KIND_SYSTEM,     // PID 0 or 4, the Windows folder, or a process that cannot be opened
} ListenerKind;

typedef struct {
  uint16_t port;
  uint32_t pid;
  ListenerKind kind;
  bool ipv4, ipv6;  // the families it listens on; both sockets make one row
  bool lan;         // other machines can reach it: bound to a wildcard or non-loopback address
  bool heron;       // heron-c.exe is among its ancestors
  bool opened;      // the process could be opened (its path is known)
  char name[RAW_TEXT];          // "node", "postgres", or the container's name
  char exe[RAW_TEXT];           // "node.exe" ("" for a container): what the hidden list holds
  char path[RAW_PATH];          // image path, "" unknown
  char folder[RAW_PATH];        // the working folder, else the program's folder, "" unknown
  char service[RAW_TEXT];       // the Windows service it belongs to, "" none
  char container_id[72];        // KIND_CONTAINER: what to stop
  char image[RAW_TEXT];         // KIND_CONTAINER: its image
  uint64_t started;             // FILETIME, 0 unknown
} Listener;

typedef struct {
  Listener *items;
  int count, cap;
} ListenerList;

// A zeroed ListenerList is a valid empty list.
// Appends a zeroed row and returns it.
Listener *listener_list_add(ListenerList *l);
void listener_list_free(ListenerList *l);
