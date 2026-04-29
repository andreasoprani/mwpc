#include "app.h"
#include "raylib.h"
#include <stdlib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

static app_t *app = NULL;

static void update_frame(void)
{
    const int exit_code = app_frame(app);

#if !defined(PLATFORM_WEB)
    if (exit_code) {
        CloseWindow();
        exit(0);
    }
#endif
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
