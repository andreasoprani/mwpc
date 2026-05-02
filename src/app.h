#ifndef APP_H
#define APP_H

#include "input.h"
#include "physics/shot.h"
#include "textures.h"
#include "world.h"

typedef enum {
    APP_STATE_MENU,
    APP_STATE_RUNNING,
    APP_STATE_PAUSED,
    APP_STATE_WIN,
    APP_STATE_LOSE,
} app_state_t;

typedef struct app {
    app_state_t state;
    float physics_time_accumulator;

    world_t *world;
    input_t input;
    textures_t textures;

    shot_t *shot;
    unsigned int shots_count;

    int window_width;
    int window_height;
    bool resized_this_frame;

} app_t;

app_t *app_setup();
void app_destroy(app_t *app);
int app_frame(app_t *app);

void app_init_shot(app_t *app);
void app_apply_shot(app_t *app);

#endif
