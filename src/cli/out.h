// The command line's text in and out: UTF-16 to and from a console, UTF-8
// bytes to and from a pipe or a file. Only cli/ includes this.
#pragma once

#include <stdbool.h>
#include <stddef.h>

void out_text(const char *utf8);
void out_line(const char *utf8);
void err_line(const char *utf8);

// One line of input without its line end. False at the end of input. Input
// that is not a console is not echoed, so a line end goes out after it.
bool in_line(char *out, size_t cap);
