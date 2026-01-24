#ifndef APP_H
#define APP_H

#include "input.h"
#include "world.h"

typedef struct app {
    world_t *world;
    input_t *input;

    bool debug;

} app_t;

app_t *app_setup();
void app_frame(app_t *app);

#endif
