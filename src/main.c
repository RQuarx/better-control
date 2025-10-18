#include <getopt.h>

#include <gtk/gtk.h>

#include "data.h"
#include "utils.h"


static void
on_activate(GtkApplication *app, struct AppData *app_data)
{
    GtkWidget *window = gtk_application_window_new(app);
    app_data->window = window;

    app_load_widgets(app_data);
    gtk_widget_show_all(app_data->window);
}


[[noreturn]]
static void
print_help_message(gchar *binary)
{
    gchar *line = g_strdup(binary);
    memset(line, '-', strlen(line));

    g_print("Usage: %s <options {param}>\n", binary);
    g_print("-------%s------------------\n\n", line);
    g_free(line);

    g_print("  Options:\n");
    g_print("    --help    -h            prints this message\n");
    g_print("    --version -V            prints version info\n");
    g_print("    --size    -s {w,h}      set the window size\n");
    g_print("    --tab     -t {name}     set the focused tab\n\n");
    g_print("  Environment Variables:\n");
    g_print("    MINIMAL_TAB: {boole}  whether to hide the notebook (false)\n");
    exit(0);
}


static gint
parse_arguments(gint argc, gchar **argv, struct AppData *app_data)
{
    gint c;
    while (true)
    {
        gint                 option_index   = 0;
        static struct option long_options[] = {
            { "help",    no_argument,       nullptr, 'h' },
            { "version", no_argument,       nullptr, 'V' },
            { "size",    required_argument, nullptr, 's' },
            { "tab",     required_argument, nullptr, 't' },
            { nullptr,   0,                 nullptr, 0   }
        };

        c = getopt_long(argc, argv, "hVs:t:", long_options, &option_index);
        if (c == -1) break;

        switch (c)
        {
        case 'h': print_help_message(*argv);
        case 'V':
            g_print("%s %s-%s\n", APP_ID, APP_VERSION, APP_GIT_HASH);
            return 0;

        case 's':
            gchar **size_parts = g_strsplit(optarg, ",", 2);
            if (!size_parts[0] || !size_parts[1])
            {
                g_warning("invalid size format (width,height)");
                break;
            }

            for (gint i = 0; i < 2; i++)
            {
                gchar *end_ptr = nullptr;
                gint64 val     = g_ascii_strtoll(size_parts[i], &end_ptr, 10);

                if (end_ptr == optarg || *end_ptr != '\0' || val > G_MAXINT
                    || val < G_MININT)
                {
                    g_warning("invalid size input: %s", optarg);
                    break;
                }

                if (i == 0)
                    app_data->arg.window.width = val;
                else
                    app_data->arg.window.height = val;
            }
            break;

        case 't': app_data->arg.focused_tab = g_strdup(optarg); break;

        case '?': return 1;
        default:
            g_print("getopt_long(): getopt returned character code 0%o", c);
            return 1;
        }
    }

    return -1;
}


static void
parse_environment_variables(struct AppData *app_data)
{
    const gchar *minimal_tab = g_getenv("MINIMAL_TAB");
    if (minimal_tab == nullptr) return;

    if (g_ascii_strcasecmp(minimal_tab, "true") == 0)
    {
        app_data->minimal_tab = true;
        return;
    }

    if (g_ascii_strcasecmp(minimal_tab, "false") == 0)
    {
        app_data->minimal_tab = false;
        return;
    }

    g_warning("invalid boolean passed to MINIMAL_TAB: %s", minimal_tab);
}


gint
main(gint argc, gchar **argv)
{
    struct AppData *app_data = g_new(struct AppData, 1);
    GtkApplication *app;
    gint            retval;
    gint            status;

    app_data->arg.window.height = -1;
    app_data->arg.window.width  = -1;
    app_data->arg.focused_tab   = nullptr;

    parse_environment_variables(app_data);
    retval = parse_arguments(argc, argv, app_data);
    if (retval > -1) return retval;

    app = gtk_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), app_data);
    status = g_application_run((GApplication *)app, 0, nullptr);
    g_object_unref(app);

    return status;
}
