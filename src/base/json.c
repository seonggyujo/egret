// Making and reading JSON values.
#include "base/json.h"

#include <stdlib.h>
#include <string.h>

#include "base/mem.h"

Json *json_new(JsonType type) {
  Json *v = xcalloc(1, sizeof *v);
  v->type = type;
  return v;
}

Json *json_new_bool(bool value) {
  Json *v = json_new(JSON_BOOL);
  v->boolean = value;
  return v;
}

void json_free(Json *v) {
  if (!v) return;
  for (int i = 0; i < v->count; i++) {
    json_free(v->items[i]);
    if (v->keys) free(v->keys[i]);
  }
  free(v->items);
  free(v->keys);
  free(v->text);
  free(v);
}

bool json_is(const Json *v, JsonType type) { return v && v->type == type; }

static int key_index(const Json *object, const char *key) {
  if (!json_is(object, JSON_OBJECT)) return -1;
  for (int i = 0; i < object->count; i++) {
    if (strcmp(object->keys[i], key) == 0) return i;
  }
  return -1;
}

Json *json_get(const Json *object, const char *key) {
  int i = key_index(object, key);
  return i < 0 ? NULL : object->items[i];
}

const char *json_string(const Json *v) { return json_is(v, JSON_STRING) ? v->text : NULL; }

bool json_number(const Json *v, double *out) {
  if (!json_is(v, JSON_NUMBER)) return false;
  *out = v->number;
  return true;
}

static void grow(Json *v) {
  if (v->count < v->cap) return;
  v->cap = v->cap ? v->cap * 2 : 4;
  v->items = xrealloc(v->items, sizeof(Json *) * (size_t)v->cap);
  if (v->type == JSON_OBJECT) v->keys = xrealloc(v->keys, sizeof(char *) * (size_t)v->cap);
}

void json_set(Json *object, const char *key, Json *value) {
  int i = key_index(object, key);
  if (i >= 0) {
    json_free(object->items[i]);
    object->items[i] = value;
    return;
  }
  grow(object);
  object->keys[object->count] = xmalloc(strlen(key) + 1);
  strcpy(object->keys[object->count], key);
  object->items[object->count++] = value;
}

void json_push(Json *array, Json *value) {
  grow(array);
  array->items[array->count++] = value;
}
