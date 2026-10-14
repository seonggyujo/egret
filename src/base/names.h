// Comparing program names and Windows paths the way Windows does for
// ASCII letters: case-insensitively. Other bytes compare exactly.
#pragma once

#include <stdbool.h>
#include <stddef.h>

bool name_equal(const char *a, const char *b);

bool text_starts_with(const char *s, const char *prefix);

// Whether hay contains the first n bytes of needle.
bool text_contains(const char *hay, const char *needle, size_t n);

// Whether path is dir or lies inside it ("C:\Windows\System32\x.exe" is
// inside "C:\WINDOWS"). An empty dir holds nothing.
bool path_inside(const char *path, const char *dir);

// The folder part of a file path, without a trailing backslash ("C:\"
// keeps its own). "" when path has no backslash.
void path_folder(const char *path, char *out, size_t cap);

// The file name part of a path ("node.exe").
const char *path_file_name(const char *path);

// Copies src into out, cut short at a whole UTF-8 character when it does
// not fit.
void text_copy(char *out, size_t cap, const char *src);
