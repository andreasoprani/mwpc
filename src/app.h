#ifndef APP_H
#define APP_H

#include "input.h"
#include "physics/shot.h"
#include "textures.h"
#include "world.h"

typedef struct app {
    world_t world;
    input_t input;
    textures_t textures;

    bool debug;

    shot_t *shot;

} app_t;

app_t *app_setup();
void app_frame(app_t *app);

void app_init_shot(app_t *app);
void app_apply_shot(app_t *app);

#endif
