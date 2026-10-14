// Programs the user took off the list, by exe name ("Discord.exe"), kept in
// the settings as one ';'-joined value. Names compare ASCII
// case-insensitively.
#pragma once

#include <stdbool.h>
#include <stddef.h>

#define HIDDEN_MAX 64

typedef struct {
  char names[HIDDEN_MAX][128];
  int count;
} HiddenList;

// From and to the settings.
void hidden_load(HiddenList *h);
// Each saves the list.
void hidden_add(HiddenList *h, const char *exe);
void hidden_remove(HiddenList *h, const char *exe);

bool hidden_contains(const HiddenList *h, const char *exe);

// The ';'-joined text and back (empty names and repeats dropped).
void hidden_parse(HiddenList *h, const char *joined);
void hidden_join(const HiddenList *h, char *out, size_t cap);

// Pointers to the names, for model/filter.h. Returns how many.
int hidden_pointers(const HiddenList *h, const char **out, int max);
