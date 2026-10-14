// Opening what a row points at, with the shell.
#pragma once

#include <stdbool.h>
#include <stdint.h>

// http://localhost:<port> in the default browser.
bool open_browser(uint16_t port);

// The folder in File Explorer.
bool open_folder(const char *folder);
