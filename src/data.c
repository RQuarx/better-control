#include "data.h"

#include <gtk/gtk.h>

#define RESOURCE_PREFIX(path) "/org/BetterDE/Better-Control" path


bool
app_load_widgets(struct AppData *app_data)
{
    GtkBuilder *builder = gtk_builder_new();
    GError     *err     = nullptr;

    if (!gtk_builder_add_from_resource(
            builder, RESOURCE_PREFIX("/assets/ui/window.xml"), &err))
    {
        g_critical("failed to load resource /assets/ui/window.xml: %s",
                   err->message);
        g_error_free(err);
        return false;
    }

    GtkWidget *box
        = GTK_WIDGET(gtk_builder_get_object(builder, "control_container"));

    gtk_container_add(GTK_CONTAINER(app_data->window), box);

    g_object_unref(builder);
    return true;
}
