// What the probes gather about the machine before it becomes rows: the
// listening sockets, the process snapshot, details of the processes that
// listen, running services and Docker containers. probe/ and docker/ fill
// a RawScan; model/rows.h turns it into rows. Plain data, no Win32 types.
#pragma once

#include <stdbool.h>
#include <stdint.h>

#define RAW_TEXT 128  // names
#define RAW_PATH 784  // a Windows path in UTF-8 (MAX_PATH UTF-16 units, 3 bytes each)

// One listening TCP socket.
typedef struct {
  uint16_t port;
  uint32_t pid;
  bool ipv6;
  bool loopback;  // 127.0.0.0/8 or ::1
  bool wildcard;  // 0.0.0.0 or ::
} RawSocket;

// One process from the system snapshot, which lists every process.
typedef struct {
  uint32_t pid, parent;
  char exe[RAW_TEXT];  // image file name: "node.exe"
} RawProcess;

// What opening one process told. Empty strings and 0 where it was refused.
typedef struct {
  uint32_t pid;
  bool opened;          // PROCESS_QUERY_LIMITED_INFORMATION was granted
  char path[RAW_PATH];  // full image path
  char cwd[RAW_PATH];   // current folder from the PEB, no trailing backslash (but "C:\")
  uint64_t started;     // creation time in FILETIME units (100 ns since 1601), 0 unknown
} RawProcInfo;

// A running Win32 service and the program its command line starts.
typedef struct {
  uint32_t pid;
  char name[RAW_TEXT];    // "postgresql-x64-18"
  char binary[RAW_PATH];  // "C:\Program Files\PostgreSQL\18\bin\pg_ctl.exe", no quotes or arguments
} RawService;

// A port a running container publishes on the host.
typedef struct {
  uint16_t public_port, private_port;
  char ip[48];  // "0.0.0.0", "::", "127.0.0.1"; "" when the engine left it out
} RawContainerPort;

typedef struct {
  char id[72];           // the full id
  char name[RAW_TEXT];   // without the leading '/'
  char image[RAW_TEXT];  // "redis:7-alpine"
  uint64_t created;      // FILETIME of its creation, 0 unknown
  RawContainerPort ports[16];
  int port_count;        // TCP ports with a public port only
} RawContainer;

// One look at the machine. Arrays are malloc'd; raw_scan_free frees them.
typedef struct {
  RawSocket *sockets;
  int socket_count;
  RawProcess *procs;
  int proc_count;
  RawProcInfo *infos;  // one per PID that owns a socket
  int info_count;
  RawService *services;
  int service_count;
  RawContainer *containers;
  int container_count;
  char windows_dir[RAW_PATH];  // "C:\Windows"
} RawScan;

void raw_scan_free(RawScan *raw);
