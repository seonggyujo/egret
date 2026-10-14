// Tags and the folder column's text.
#include "ui/row_texts.h"

#include <stdio.h>
#include <stdlib.h>

#include "app/strings.h"
#include "app/theme.h"
#include "model/text.h"

// The user's home folder in UTF-8, read once.
static const char *home(void) {
  static char path[RAW_PATH];
  static int done;
  if (!done) {
    const char *profile = getenv("USERPROFILE");
    snprintf(path, sizeof path, "%s", profile ? profile : "");
    done = 1;
  }
  return path;
}

int row_tags(const Listener *l, RowTag out[2]) {
  const ThemeColors *t = theme();
  const Strings *s = strings();
  int n = 0;
  switch (l->kind) {
    case KIND_CONTAINER: out[n++] = (RowTag){"docker", t->docker}; break;
    case KIND_SERVICE: out[n++] = (RowTag){s->kind_service, t->text2}; break;
    case KIND_SYSTEM: out[n++] = (RowTag){s->kind_system, t->dim}; break;
    case KIND_WSL: out[n++] = (RowTag){s->kind_wsl, t->text2}; break;
    default:
      if (l->heron) out[n++] = (RowTag){"heron-c", t->heron};
      break;
  }
  if (l->lan && l->kind != KIND_SYSTEM) out[n++] = (RowTag){"LAN", t->lan};
  return n;
}

void row_folder_text(const Listener *l, char *out, size_t cap) {
  const Strings *s = strings();
  if (l->kind == KIND_CONTAINER) {
    snprintf(out, cap, "%s", l->image);
  } else if (l->folder[0]) {
    text_short_path(out, cap, l->folder, home());
  } else if (l->pid == 4) {
    snprintf(out, cap, "%s", s->kernel);
  } else if (l->service[0]) {
    snprintf(out, cap, "%s (%s)", s->no_access, l->service);
  } else {
    snprintf(out, cap, "%s", l->pid == 0 ? "-" : s->no_access);
  }
}
