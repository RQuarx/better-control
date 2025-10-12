#pragma once
#include <stdio.h>

typedef struct _GtkApplication GtkApplication; //NOLINT
typedef struct _GtkWindow      GtkWindow;      //NOLINT


struct App
{
    GtkApplication *app;
    GtkWindow      *window;

    int retval;

    struct
    {
        FILE *config;
        FILE *cache;
    } path;
};


/**
 * initialise @p context and parses command-line arguments
 * -------------------------------------------------------
 *
 * returns true on success or false on failure
 */
bool app_init(struct App *context, int argc, char **argv);


/**
 * starts the application
 */
void app_start(struct App *context);


/**
 * stops the application, and frees all memory inside @p context
 */
int app_stop(struct App *context);
