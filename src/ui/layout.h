// Where everything in the list view is, in client pixels, so painting, the
// mouse and the tooltip agree. The settings view has settings_layout.h.
#pragma once

#include "ui/app.h"

#define MAX_ROWS 10  // more scroll

// What the mouse is over. Settings parts start at PART_SETTINGS.
enum {
  PART_NONE,
  PART_GEAR,
  PART_BACK,
  PART_STATUS_LINK,
  PART_ROW,
  PART_BROWSER,
  PART_FOLDER,
  PART_STOP,
  PART_CANCEL,
  PART_CONFIRM,
  PART_FOLDER_TEXT,
  PART_TAGS,
  PART_SETTINGS = 100,
};

Box layout_header(const App *a);
Box layout_mark(const App *a);
Box layout_gear(const App *a);
Box layout_back(const App *a);
Box layout_search(const App *a);
Box layout_colhead(const App *a);
// Below the header (and the column titles when rows show), above the status bar.
Box layout_body(const App *a);
Box layout_status(const App *a);

// Rows the list shows at once.
int layout_rows_shown(const App *a);
// A visible row's box; zero width when it is scrolled out of view.
Box layout_row(const App *a, int visible_index);
// The visible index under (x, y), or -1.
int layout_hit_row(const App *a, int x, int y);

typedef struct {
  Box port, name, folder, tags, up;  // the columns
  Box browser, folder_button, stop;  // buttons on the row under the mouse
  Box text, cancel, confirm;         // the yellow confirm row
} RowParts;

void layout_row_parts(const App *a, Box row, RowParts *p);
