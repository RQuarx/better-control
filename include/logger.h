#pragma once
#include <stdio.h>
#include <stdlib.h>


#define BC_MASSERT(expr, msg)                                      \
    if ((expr))                                                    \
    {                                                              \
        fprintf(stderr, "assertion (" #expr ") failed at %s:%d\n", \
                __FILE__, __LINE__);                               \
        fprintf(stderr, "  what(): %s\n", msg);                    \
        abort();                                                   \
    }

#define BC_ASSERT(expr)                                            \
    if ((expr))                                                    \
    {                                                              \
        fprintf(stderr, "assertion (" #expr ") failed at %s:%d\n", \
                __FILE__, __LINE__);                               \
        abort();                                                   \
    }


[[maybe_unused]]
static void
log_sentinel_func__()
{
    printf("");
    abort();
}


/**
 * parse log option from the command line
 */
void log_parse_option(const char *arg);


/**
 * sets the log level from a string @p level
 * -----------------------------------------
 *
 * @p level can be a string "debug", "info", "warn", "error",
 * (non-case sensitive) or an integer 0, 1, 2, 3
 *
 * the function returns true if @p level is a valid level
 */
bool log_set_level(const char *level);


/**
 * sets the log file to @p path
 * ----------------------------
 *
 * return true on success, or false on failure
 */
bool log_set_log_file(const char *path);


void log_close_log_file();


typedef enum
{
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_WARN,
    LEVEL_ERROR,
} LogLevel;


[[gnu::format(printf, 4, 5)]]
void log_write(LogLevel level, const char *file, int line, const char *fmt,
               ...);

#define LOG_WRITE(level, ...) \
    log_write(level, __FILE__, __LINE__, __VA_ARGS__)
