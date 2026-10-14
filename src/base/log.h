// Leveled log lines on stderr. Debug builds are console programs, so the
// lines show in the terminal that started them. EGRET_LOG picks the level
// (error, warn, info, debug, trace); the default is debug in Debug builds,
// warn otherwise.
#pragma once

#include <stddef.h>

typedef enum { LOG_ERROR, LOG_WARN, LOG_INFO, LOG_DEBUG, LOG_TRACE } LogLevel;

void log_init(void);
// The CLI keeps its output clean: only errors then.
void log_set_level(LogLevel level);
int log_enabled(LogLevel level);

// The UCRT printf understands C99 formats such as %zu; gcc calls that
// checker gnu_printf, clang (used by clangd) calls it printf.
#if defined(__clang__)
__attribute__((format(printf, 3, 4)))
#elif defined(__GNUC__)
__attribute__((format(gnu_printf, 3, 4)))
#endif
void log_write(LogLevel level, const char *scope, const char *fmt, ...);

#define log_error(scope, ...) log_write(LOG_ERROR, scope, __VA_ARGS__)
#define log_warn(scope, ...) log_write(LOG_WARN, scope, __VA_ARGS__)
#define log_info(scope, ...) log_write(LOG_INFO, scope, __VA_ARGS__)
#define log_debug(scope, ...) log_write(LOG_DEBUG, scope, __VA_ARGS__)
#define log_trace(scope, ...) log_write(LOG_TRACE, scope, __VA_ARGS__)
