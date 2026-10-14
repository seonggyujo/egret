// Turning one look at the machine into rows: one per (port, PID), its IPv4
// and IPv6 sockets merged, then classified and sorted.
#include "model/rows.h"

#include <stdlib.h>

#include "model/rows_parts.h"

static Listener *row_for(ListenerList *out, uint16_t port, uint32_t pid) {
  for (int i = 0; i < out->count; i++) {
    if (out->items[i].port == port && out->items[i].pid == pid) return &out->items[i];
  }
  Listener *row = listener_list_add(out);
  row->port = port;
  row->pid = pid;
  return row;
}

static int by_port_then_pid(const void *a, const void *b) {
  const Listener *x = a, *y = b;
  if (x->port != y->port) return x->port < y->port ? -1 : 1;
  return x->pid < y->pid ? -1 : x->pid > y->pid;
}

void rows_build(const RawScan *raw, ListenerList *out) {
  out->count = 0;
  for (int i = 0; i < raw->socket_count; i++) {
    const RawSocket *s = &raw->sockets[i];
    Listener *row = row_for(out, s->port, s->pid);
    if (s->ipv6) row->ipv6 = true;
    else row->ipv4 = true;
    if (s->wildcard || !s->loopback) row->lan = true;
  }
  for (int i = 0; i < out->count; i++) rows_classify(raw, &out->items[i]);
  qsort(out->items, (size_t)out->count, sizeof *out->items, by_port_then_pid);
}
