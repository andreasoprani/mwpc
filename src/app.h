#ifndef APP_H
#define APP_H

#include "input.h"
#include "world.h"

typedef struct App {
    World *world;
    Input *input;

    bool debug;
    bool debugRun;

} App;

App *app_setup();
void app_frame(App *app);

#endif
