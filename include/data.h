#pragma once
#include <glib.h>

typedef struct _GtkWidget GtkWidget; //NOLINT


struct AppData
{
    struct
    {
        struct
        {
            gint width;
            gint height;
        } window;

        gchar *focused_tab;
    } arg;

    bool minimal_tab;


    GtkWidget  *window;
    GHashTable *tabs;
};


bool app_load_widgets(struct AppData *app_data);
