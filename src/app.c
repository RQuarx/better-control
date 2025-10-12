#include "app.h"

#include <getopt.h>
#include <gtk/gtk.h>

#include "logger.h"


static void app_activate(struct App *context);
static void print_help_message(int help_type);
static void handle_cli_arg(int argc, char **argv, char **style_path);


bool
app_init(struct App *context, int argc, char **argv)
{
    if (context == nullptr) return false;

    context->app = gtk_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(context->app, "activate", G_CALLBACK(app_activate),
                     context);

    char *style_path = nullptr;
    handle_cli_arg(argc, argv, &style_path);

    LOG_WRITE(LEVEL_DEBUG, "test");
    LOG_WRITE(LEVEL_INFO, "test");
    LOG_WRITE(LEVEL_WARN, "test");
    LOG_WRITE(LEVEL_ERROR, "test");

    return true;
}


void
app_start(struct App *context)
{
    context->retval
        = g_application_run(G_APPLICATION(context->app), 0, nullptr);
    g_object_unref(context->app);
}


int
app_stop(struct App *context)
{
    return context->retval;
}


static void
app_activate(struct App *context)
{
}


static void
print_help_message(int help_type)
{
    if (help_type == 0)
        printf(
            "Usage: kacewm <options {param}>\n"
            "-------------------------------\n"
            "  Options:\n"
            "    --help    -h                  Shows this message\n"
            "    --version -V                  Shows kacewm's version\n"
            "    --log     -l {level|path}     Sets the log level and or file\n"
            "    --style   -s {path}           Specify a non-default css "
            "file\n"
            "    --clear-cache                 Clear cache file\n");
    else if (help_type == 1)
        printf(
            "Logger:\n"
            "    Better-Control's logger will log to stderr, and a file,\n"
            "  which is passed from the command line argument or an envvar\n"
            "  \"LOG_FILE\". The logger have 4 valid levels that can be "
            "passed\n"
            "  with the command line argument or the envvar \"LOG_LEVEL\", "
            "that is\n"
            "    0. DEBUG\n"
            "    1. INFO\n"
            "    2. WARN\n"
            "    3. ERROR\n"
            "  Using a log level of DEBUG means that every log with the level "
            "of\n"
            "  DEBUG and above will be printed to stderr.\n\n"
            "Command Line:\n"
            "    The command line parameter passed to { --log,-l } will be "
            "parsed\n"
            "  in a way such that ',', '|''s will be used as separators.\n"
            "  That means `--log=log.txt,DEBUG` will become LOG_LEVEL=DEBUG\n"
            "  and LOG_FILE=log.txt.\n");
    exit(0);
}


static void
handle_cli_arg(int argc, char **argv, char **style_path)
{
    while (true)
    {
        int c;
        int opt_index = 0;

        static struct option long_option[] = {
            { "help",        no_argument,       nullptr, 'h' },
            { "version",     no_argument,       nullptr, 'V' },
            { "log",         optional_argument, nullptr, 'l' },
            { "clear-cache", no_argument,       nullptr, 0   },
            { "style",       optional_argument, nullptr, 's' },
            { nullptr,       0,                 nullptr, 0   },
        };

        c = getopt_long(argc, argv, "hVl:s:", long_option, &opt_index);
        if (c == -1) break;

        switch (c)
        {
        case 'h': print_help_message(0);
        case 'V': fprintf(stderr, "%s %s\n", APP_ID, APP_VERSION); exit(0);
        case 'l':
            if (optarg == nullptr || optarg[0] == '\0') print_help_message(1);

            log_parse_option(optarg);
            break;
        case 's':
            if (optarg == nullptr)
            {
                fprintf(stderr,
                        "--style, -s requires a path pointing to a css file\n");
                exit(1);
            }
            *style_path = g_strdup(optarg);
            break;
        case '?': exit(1);
        default:  break;
        }
    }
}
