#include "app.h"
#include "raylib.h"
#include <stdlib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

static app_t *app = NULL;

static void update_frame(void)
{
    app_frame(app);
}

int main(void)
{
    app = app_setup();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(update_frame, 0, 1);
#else
    while (!WindowShouldClose()) {
        update_frame();
    }

    CloseWindow();
#endif

    return 0;
}
