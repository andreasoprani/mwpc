#include "app.h"
#include "raylib.h"
#include <stdlib.h>

int main()
{
    App *app = app_setup();

    while (WindowShouldClose() == false) {
        app_frame(app);
    }

    CloseWindow();

    return 0;
}
