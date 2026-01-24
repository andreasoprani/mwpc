#include "app.h"
#include "raylib.h"
#include <stdlib.h>

int main()
{
    app_t *app = app_setup();

    while (WindowShouldClose() == false) {
        app_frame(app);
    }

    CloseWindow();

    return 0;
}
