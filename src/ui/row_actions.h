// What a row's buttons, keys and right-click menu do: open it in the
// browser, open its folder, copy the port, take its program off the list,
// and stopping in two presses (the first turns the row yellow, the second
// stops it). Services stop through the UAC prompt.
#pragma once

#include <stdbool.h>

#include "model/listener.h"
#include "ui/app.h"

bool row_can_browse(const Listener *l);
bool row_has_folder(const Listener *l);
// Not system rows and not WSL's forwarder.
bool row_can_stop(const Listener *l);

void row_open_browser(App *a, int visible_index);
void row_open_folder(App *a, int visible_index);
// First press: the yellow confirm row (or a note when it cannot be stopped).
void row_ask_stop(App *a, int visible_index);
// Second press.
void row_confirm_stop(App *a);
void row_cancel_stop(App *a);
// Whether the confirm row is this one.
bool row_is_confirming(const App *a, const Listener *l);
bool row_is_stopping(const App *a, const Listener *l);

// The right-click menu at a screen point.
void row_menu(App *a, int visible_index, POINT screen);
