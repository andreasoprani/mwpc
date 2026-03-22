#include "app.h"
#include "constants.h"
#include "physics/ball.h"
#include "raylib.h"
#include "raymath.h"
#include "render.h"
#include "textures.h"
#include "world.h"
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

void toggle_debug(app_t *app)
{
    const bool curr_debug = app->debug;
    app->debug = !curr_debug;
    printf("Debug mode: %s\n", app->debug ? "ON" : "OFF");
}

table_t *table_setup(app_t *app)
{
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();

    const float table_height = sh - 2 * TABLE_H_PAD;
    const float table_width = table_height / TABLE_RATIO;

    float w_padding = (sw - table_width) / 2;

    Vector2 tl = {w_padding, TABLE_H_PAD};
    Vector2 tr = {sw - w_padding, TABLE_H_PAD};
    Vector2 bl = {w_padding, sh - TABLE_H_PAD};
    Vector2 br = {sw - w_padding, sh - TABLE_H_PAD};

    Vector2 vertices[4] = {tl, tr, br, bl};

    table_t *table = table_create(4, vertices);

    return table;
}

world_t *world_setup(app_t *app)
{
    table_t *table = table_setup(app);

    world_t *world = world_create(table);

    static const planet_t other_planets[] = {MERCURY, VENUS, MARS};
    world_place_all_balls(world, EARTH, other_planets, 3);

    return world;
}

app_t *app_setup()
{
    app_t *app = malloc(sizeof(app_t));
    app->input = input_create();
    app->shot = NULL;

    app->debug = false;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Milky Way Pool Club");
    app->textures = textures_setup();
    SetTargetFPS(120);
    app->world = world_setup(app);
    return app;
}

void apply_app_inputs(app_t *app)
{
    if (app->input->key_g_pressed)
        world_toggle_gravity(app->world);

    if (app->input->key_d_pressed)
        toggle_debug(app);

    if (app->input->mouse_left_pressed)
        app_init_shot(app);

    if (app->input->mouse_left_released)
        app_apply_shot(app);

    if (app->shot != NULL)
        app->shot->end = app->input->mouse_position;
}

void app_frame(app_t *app)
{
    float dt = GetFrameTime();

    input_update(app->input);
    apply_app_inputs(app);

    if ((!app->debug || app->input->key_space_pressed) && app->shot == NULL) {
        world_update(app->world, dt);
    }
    render_world(app);
}

void app_init_shot(app_t *app)
{
    app->shot = shot_create(app->input->mouse_position);
}

void app_apply_shot(app_t *app)
{
    if (app->shot == NULL)
        return;

    ball_t *ball = app->world->balls[0];

    Vector2 shot_vec = shot_vector(app->shot);

    ball_reset_impulses(ball);
    ball_apply_impulse_linear(ball,
                              Vector2Scale(shot_vec, SHOT_IMPULSE_MULTIPLIER));

    free(app->shot);
    app->shot = NULL;
}
