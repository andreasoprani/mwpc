#include "app.h"
#include "constants.h"
#include "input.h"
#include "physics/ball.h"
#include "physics/planets.h"
#include "raylib.h"
#include "raymath.h"
#include "render.h"
#include "textures.h"
#include "world.h"
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

app_t *app_setup()
{
    app_t *app = malloc(sizeof(app_t));

    app->state = APP_STATE_MENU;

    input_reset(&app->input);
    app->shot = NULL;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Milky Way Pool Club");

    SetExitKey(KEY_NULL);

    textures_setup(&app->textures);
    SetTargetFPS(120);

    app->world = world_create();

    return app;
}

void app_destroy(app_t *app)
{
    free(app->world);
    free(app);
}

void apply_game_inputs(app_t *app)
{
    if (app->input.key_g_pressed)
        world_toggle_gravity(app->world);

    if (app->input.mouse_left_pressed)
        app_init_shot(app);

    if (app->input.mouse_left_released)
        app_apply_shot(app);

    if (app->shot != NULL)
        app->shot->end = app->input.mouse_position;

    if (app->input.key_esc_pressed)
        app->state = APP_STATE_PAUSED;
}

void running_frame(app_t *app)
{
    float dt = GetFrameTime();

    apply_game_inputs(app);

    if (app->shot == NULL) {
        world_update(app->world, dt);
    }

    bool earth_ball_exists = false;
    for (unsigned int i = 0; i < app->world->balls_count; i++) {
        if (app->world->balls[i].planet == EARTH) {
            earth_ball_exists = true;
            break;
        }
    }
    if (!earth_ball_exists) {
        app->state = APP_STATE_LOSE;
    } else if (app->world->balls_count == 1) {
        app->state = APP_STATE_WIN;
    }
}

void pause_frame(app_t *app)
{
    if (app->input.key_space_pressed) {
        app->state = APP_STATE_RUNNING;
    }
}

void end_game_frame(app_t *app)
{
    if (app->input.key_space_pressed) {
        app->state = APP_STATE_RUNNING;
        free(app->world);
        app->world = world_create();
    }
}

int app_frame(app_t *app)
{
    input_update(&app->input);
    switch (app->state) {
    case APP_STATE_RUNNING:
        running_frame(app);
        break;
    case APP_STATE_MENU:
    case APP_STATE_PAUSED:
        pause_frame(app);
        break;
    case APP_STATE_WIN:
    case APP_STATE_LOSE:
        end_game_frame(app);
        break;
    }

    render(app);

    return 0;
}

void app_init_shot(app_t *app)
{
    app->shot = shot_create(app->input.mouse_position);
}

void app_apply_shot(app_t *app)
{
    if (app->shot == NULL)
        return;

    ball_t *ball = &app->world->balls[0];

    Vector2 shot_vec = shot_vector(app->shot);

    ball_reset_impulses(ball);
    ball_apply_impulse_linear(ball,
                              Vector2Scale(shot_vec, SHOT_IMPULSE_MULTIPLIER));

    free(app->shot);
    app->shot = NULL;
}
