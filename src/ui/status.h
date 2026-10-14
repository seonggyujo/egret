// The status bar at the bottom. In the list: the last stop's outcome for a
// few seconds, else how many ports listen and how many are hidden, with a
// link to show or hide the rest. In the settings: the version and a link to
// the settings folder.
#pragma once

#include "ui/app.h"

void status_paint(App *a);
// The link's box (empty when there is none).
Box status_link_box(App *a);
void status_link_click(App *a);
