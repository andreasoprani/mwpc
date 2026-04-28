#include "app.h"
#include "raylib.h"
#include <stdlib.h>

int main()
{
    app_t *app = app_setup();

    while (!WindowShouldClose()) {
        const int exit_code = app_frame(app);
        if (exit_code)
            break;
    }

    CloseWindow();

    return 0;
}
