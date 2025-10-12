#include "app.h"


int
main(int argc, char **argv)
{
    struct App app;

    app_init(&app, argc, argv);
    return app_stop(&app);
}
