// Reading the IPv4 and IPv6 listener tables from the IP helper API.
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#include "probe/sockets.h"

#include <stdlib.h>
#include <string.h>

#include "base/log.h"
#include "base/mem.h"

// The table, malloc'd, or NULL. It can grow between the size query and the
// read, so this asks again until it fits.
static void *read_table(ULONG family) {
  DWORD size = 0;
  void *table = NULL;
  for (int tries = 0; tries < 4; tries++) {
    DWORD err = GetExtendedTcpTable(table, &size, FALSE, family, TCP_TABLE_OWNER_PID_LISTENER, 0);
    if (err == NO_ERROR && table) return table;
    if (err != ERROR_INSUFFICIENT_BUFFER && err != NO_ERROR) {
      log_warn("sockets", "GetExtendedTcpTable(%lu) failed: %lu", family, err);
      break;
    }
    table = xrealloc(table, size ? size : 1);
  }
  free(table);
  return NULL;
}

// Ports come in network byte order in the low 16 bits.
static uint16_t port_of(DWORD raw_port) { return (uint16_t)(((raw_port & 0xFF) << 8) | ((raw_port >> 8) & 0xFF)); }

static RawSocket *add(RawScan *raw, int *cap) {
  if (raw->socket_count == *cap) {
    *cap = *cap ? *cap * 2 : 64;
    raw->sockets = xrealloc(raw->sockets, sizeof *raw->sockets * (size_t)*cap);
  }
  RawSocket *s = &raw->sockets[raw->socket_count++];
  memset(s, 0, sizeof *s);
  return s;
}

static void add_v4(RawScan *raw, int *cap, const MIB_TCPTABLE_OWNER_PID *t) {
  for (DWORD i = 0; i < t->dwNumEntries; i++) {
    const MIB_TCPROW_OWNER_PID *r = &t->table[i];
    const unsigned char *a = (const unsigned char *)&r->dwLocalAddr;  // network byte order
    RawSocket *s = add(raw, cap);
    s->port = port_of(r->dwLocalPort);
    s->pid = r->dwOwningPid;
    s->loopback = a[0] == 127;
    s->wildcard = r->dwLocalAddr == 0;
  }
}

static void add_v6(RawScan *raw, int *cap, const MIB_TCP6TABLE_OWNER_PID *t) {
  static const unsigned char LOOPBACK[16] = {[15] = 1}, ANY[16] = {0};
  for (DWORD i = 0; i < t->dwNumEntries; i++) {
    const MIB_TCP6ROW_OWNER_PID *r = &t->table[i];
    RawSocket *s = add(raw, cap);
    s->port = port_of(r->dwLocalPort);
    s->pid = r->dwOwningPid;
    s->ipv6 = true;
    s->loopback = memcmp(r->ucLocalAddr, LOOPBACK, 16) == 0;
    s->wildcard = memcmp(r->ucLocalAddr, ANY, 16) == 0;
  }
}

bool probe_sockets(RawScan *raw) {
  int cap = 0;
  MIB_TCPTABLE_OWNER_PID *v4 = read_table(AF_INET);
  MIB_TCP6TABLE_OWNER_PID *v6 = read_table(AF_INET6);
  if (v4) add_v4(raw, &cap, v4);
  if (v6) add_v6(raw, &cap, v6);
  bool ok = v4 || v6;
  free(v4);
  free(v6);
  return ok;
}
