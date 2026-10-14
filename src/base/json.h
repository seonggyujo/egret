// JSON values, parsed and read (the Docker engine's replies). Objects keep
// their keys in the order they came; a duplicate key replaces the value in
// its first place. Strings are UTF-8 with a length (they may hold \u0000).
#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef enum { JSON_NULL, JSON_BOOL, JSON_NUMBER, JSON_STRING, JSON_ARRAY, JSON_OBJECT } JsonType;

typedef struct Json {
  JsonType type;
  bool boolean;
  double number;
  char *text;  // STRING: the value; NUMBER: as written
  size_t len;  // STRING: bytes in text
  int count, cap;
  struct Json **items;  // ARRAY: the items; OBJECT: the values
  char **keys;          // OBJECT: the keys
} Json;

// One JSON value with only whitespace around it; NULL for anything else.
Json *json_parse(const char *text, size_t len);
void json_free(Json *v);

// Reading. Every function takes NULL and then finds nothing.
bool json_is(const Json *v, JsonType type);
Json *json_get(const Json *object, const char *key);
// The string, NULL unless v is one.
const char *json_string(const Json *v);
bool json_number(const Json *v, double *out);

// Building, for the parser. json_set and json_push own the value after.
Json *json_new(JsonType type);
Json *json_new_bool(bool value);
void json_set(Json *object, const char *key, Json *value);
void json_push(Json *array, Json *value);
