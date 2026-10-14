// What a row says beyond its name: the folder column's text and the tags
// (heron-c, docker, service, system, WSL, LAN) with their colors. Shared by
// the painting and the tooltip.
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "model/listener.h"

typedef struct {
  const char *text;
  uint32_t color;
} RowTag;

// Up to two tags; returns how many.
int row_tags(const Listener *l, RowTag out[2]);
// The folder with ~ for the home folder, a container's image, or why a
// system row has none.
void row_folder_text(const Listener *l, char *out, size_t cap);
