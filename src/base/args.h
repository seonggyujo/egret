// This program's command line as UTF-8 strings, split the Windows way.
#pragma once

// argv[0] is the program. The array and its strings stay for the life of
// the process.
char **args_utf8(int *argc);
