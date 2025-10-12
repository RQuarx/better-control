#include "logger.h"

#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#include <linux/limits.h>
#include <sys/time.h>

#include <glib.h>


static const char *LOG_LEVEL_STRING[4] = { [LEVEL_DEBUG] = "DEBUG",
                                           [LEVEL_INFO]  = "INFO ",
                                           [LEVEL_WARN]  = "WARN ",
                                           [LEVEL_ERROR] = "ERROR" };

#ifdef COLORED_LOG_MESSAGE

static const char *LOG_LEVEL_COLORS[4] = { [LEVEL_DEBUG] = "\x1b[0;36m",
                                           [LEVEL_INFO]  = "\x1b[0;32m",
                                           [LEVEL_WARN]  = "\x1b[0;33m",
                                           [LEVEL_ERROR] = "\x1b[0;31m" };

static const char *COLOR_RESET = "\033[0;0m";
static const char *COLOR_BOLD  = "\033[1m";

#endif


LogLevel LOG_LEVEL_THRESHOLD = LEVEL_INFO;
FILE    *LOG_FILE            = nullptr;


static void
print_time(FILE *stream)
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    struct tm tm;
    localtime_r(&tv.tv_sec, &tm);

    long ms = tv.tv_usec / 1000;
    fprintf(stream, "%02d:%02d.%03ld", tm.tm_min, tm.tm_sec, ms);
}


void
log_parse_option(const char *arg)
{
    char *copy = g_strdup(arg);
    char *sep  = strpbrk(copy, ",|");

    if (sep != NULL)
    {
        *sep               = '\0';
        const char *first  = copy;
        const char *second = sep + 1;

        bool level_ok = log_set_level(first);
        bool file_ok  = log_set_log_file(second);

        if (!level_ok || !file_ok)
        {
            level_ok = log_set_level(second);
            file_ok  = log_set_log_file(first);
        }

        if (!level_ok && !file_ok)
        {
            LOG_WRITE(LEVEL_ERROR, "invalid log level or file: '%s' and '%s'",
                      first, second);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        if (!log_set_level(copy))
        {
            if (!log_set_log_file(copy))
            {
                LOG_WRITE(LEVEL_ERROR, "invalid log level or file path: '%s'",
                          copy);
                exit(EXIT_FAILURE);
            }
        }
    }

    g_free(copy);
}


bool
log_set_level(const char *level)
{
    if (level == nullptr || level[0] == '\0')
    {
        LOG_WRITE(LEVEL_WARN, "empty log level");
        return false;
    }

    errno = 0;
    char *endptr;
    long  value = strtol(level, &endptr, 10);
    if (errno == EINVAL || errno == ERANGE) goto string_level;

    if (value < 0 || value > 3) return false;

    LOG_LEVEL_THRESHOLD = value;
    return true;

string_level:

    size_t pos = 5;
    for (size_t i = 0; i < 4; i++)
        if (strcasecmp(LOG_LEVEL_STRING[i], level) == 0)
        {
            pos = i;
            break;
        }

    if (pos == 5) return false;

    LOG_LEVEL_THRESHOLD = pos;
    return true;
}


bool
log_set_log_file(const char *path)
{
    LOG_FILE = fopen(path, "a");
    if (LOG_FILE == nullptr)
    {
        LOG_WRITE(LEVEL_ERROR, "failed to open log file at %s: %s", path,
                  strerror(errno));
        return false;
    }

    return true;
}


void
log_close_log_file()
{
    if (LOG_FILE == nullptr) return;
    fclose(LOG_FILE);
}


constexpr size_t MAX_LOG_MESSAGE = PATH_MAX;

void
log_write(LogLevel level, const char *file, int line, const char *fmt, ...)
{
    char msg[PATH_MAX];

    va_list ap;
    va_start(ap, fmt);
    if (vsnprintf(msg, MAX_LOG_MESSAGE, fmt, ap) < 0)
    {
        fprintf(stderr, "failed to format log message: %s", strerror(errno));

        log_close_log_file();
        exit(EXIT_FAILURE);
    }
    va_end(ap);

    if (LOG_FILE != nullptr)
    {
        fprintf(LOG_FILE, "[");
        print_time(LOG_FILE);
        fprintf(LOG_FILE, " %s at %s:%d]: %s\n", LOG_LEVEL_STRING[level], file,
                line, msg);
        fflush(LOG_FILE);
    }

    if (LOG_LEVEL_THRESHOLD <= level)
    {
#ifndef COLORED_LOG_MESSAGE
        fprintf(stderr, "[");
        print_time(stderr);
        fprintf(stderr, "%s at %s:%d]: %s\n", LOG_LEVEL_STRING[level], file,
                line, msg);
#else
        fprintf(stderr, "[%s", COLOR_BOLD);
        print_time(stderr);
        fprintf(stderr, " %s%s %sat %s%s:%d%s]: %s\n", LOG_LEVEL_COLORS[level],
                LOG_LEVEL_STRING[level], COLOR_RESET, COLOR_BOLD, file, line,
                COLOR_RESET, msg);
#endif
    }
}
