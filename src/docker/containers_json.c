// From the Engine API's JSON to RawContainer: the name without its '/',
// the image, the creation time and each TCP port published on the host
// (one entry per public port, though Docker lists IPv4 and IPv6 apart).
#include "docker/containers_json.h"

#include <string.h>

#include "base/json.h"
#include "base/mem.h"
#include "base/names.h"

// Seconds since 1970 to FILETIME units since 1601.
#define UNIX_EPOCH_AS_FILETIME 116444736000000000ULL
#define TICKS_PER_SECOND 10000000ULL

static void copy_string(char *out, size_t cap, const Json *v) {
  const char *s = json_string(v);
  text_copy(out, cap, s ? s : "");
}

static bool has_port(const RawContainer *c, uint16_t port) {
  for (int i = 0; i < c->port_count; i++) {
    if (c->ports[i].public_port == port) return true;
  }
  return false;
}

static void add_port(RawContainer *c, const Json *p) {
  double pub = 0, priv = 0;
  const char *type = json_string(json_get(p, "Type"));
  if (!json_number(json_get(p, "PublicPort"), &pub) || pub <= 0 || pub > 65535) return;
  if (type && strcmp(type, "tcp") != 0) return;
  if (has_port(c, (uint16_t)pub) || c->port_count == (int)(sizeof c->ports / sizeof c->ports[0])) return;
  RawContainerPort *out = &c->ports[c->port_count++];
  out->public_port = (uint16_t)pub;
  out->private_port = json_number(json_get(p, "PrivatePort"), &priv) ? (uint16_t)priv : 0;
  copy_string(out->ip, sizeof out->ip, json_get(p, "IP"));
}

static void read_container(RawContainer *c, const Json *v) {
  memset(c, 0, sizeof *c);
  copy_string(c->id, sizeof c->id, json_get(v, "Id"));
  const Json *names = json_get(v, "Names");
  const char *name = json_is(names, JSON_ARRAY) && names->count ? json_string(names->items[0]) : NULL;
  text_copy(c->name, sizeof c->name, name ? (name[0] == '/' ? name + 1 : name) : "");
  copy_string(c->image, sizeof c->image, json_get(v, "Image"));
  double created = 0;
  if (json_number(json_get(v, "Created"), &created) && created > 0) {
    c->created = UNIX_EPOCH_AS_FILETIME + (uint64_t)created * TICKS_PER_SECOND;
  }
  const Json *ports = json_get(v, "Ports");
  for (int i = 0; json_is(ports, JSON_ARRAY) && i < ports->count; i++) add_port(c, ports->items[i]);
}

int docker_parse_containers(const char *json, size_t len, RawContainer **out) {
  *out = NULL;
  Json *list = json_parse(json, len);
  if (!json_is(list, JSON_ARRAY)) {
    json_free(list);
    return -1;
  }
  int n = 0;
  if (list->count) *out = xcalloc((size_t)list->count, sizeof **out);
  for (int i = 0; i < list->count; i++) {
    if (json_is(list->items[i], JSON_OBJECT)) read_container(&(*out)[n++], list->items[i]);
  }
  json_free(list);
  return n;
}
