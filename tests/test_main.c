// Runs every unit test and exits non-zero if any CHECK failed.
#include <stdio.h>

#include "check.h"

int g_failures;

#define TESTS(X)                   \
  X(test_rows_merge_and_sort)      \
  X(test_rows_folder_in_windows)   \
  X(test_rows_system)              \
  X(test_rows_service_postgres)    \
  X(test_rows_container_and_wsl)   \
  X(test_filter)                   \
  X(test_texts)                    \
  X(test_http_parse)               \
  X(test_containers_json)          \
  X(test_engine_pipes)             \
  X(test_hotkey)                   \
  X(test_hidden_list)              \
  X(test_path_list)                \
  X(test_cli_args)                 \
  X(test_text_columns)             \
  X(test_service_program)

#define DECLARE(name) void name(void);
TESTS(DECLARE)

#define RUN(name)                                                  \
  do {                                                             \
    int before = g_failures;                                       \
    name();                                                        \
    if (g_failures != before) fprintf(stderr, "  in %s\n", #name); \
    count++;                                                       \
  } while (0);

int main(void) {
  int count = 0;
  TESTS(RUN)
  printf("%d tests, %d failed checks\n", count, g_failures);
  return g_failures ? 1 : 0;
}
