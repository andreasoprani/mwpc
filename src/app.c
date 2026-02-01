#include "app.h"
#include "input.h"
#include "physics/ball.h"
#include "physics/constants.h"
#include "physics/shot.h"
#include "raylib.h"
#include "raymath.h"
#include "render.h"
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

world_t *world_setup(app_t *app)
{
    int wall_padding = 50;

    Vector2 tl = {wall_padding, wall_padding};
    Vector2 tr = {GetScreenWidth() - wall_padding, wall_padding};
    Vector2 bl = {wall_padding, GetScreenHeight() - wall_padding};
    Vector2 br = {GetScreenWidth() - wall_padding,
                  GetScreenHeight() - wall_padding};

    Vector2 vertices[4] = {tl, tr, br, bl};

    table_t *table = table_create(4, vertices);

    world_t *world = world_create(table);

    // 0, white ball
    // TODO: fix with real ball position
    world_add_ball_at_position(
        world, (Vector2) {GetScreenWidth() / 2, GetScreenHeight() * 3 / 4});

    // TODO: fix positions
    world_add_ball_at_position(
        world, (Vector2) {GetScreenWidth() / 2, GetScreenHeight() * 1 / 4});
    world_add_ball_at_position(
        world, (Vector2) {GetScreenWidth() / 2 + BALL_DEFAULT_RADIUS,
                          GetScreenHeight() * 1 / 4 - BALL_DEFAULT_RADIUS * 2});
    world_add_ball_at_position(
        world, (Vector2) {GetScreenWidth() / 2 - BALL_DEFAULT_RADIUS,
                          GetScreenHeight() * 1 / 4 - BALL_DEFAULT_RADIUS * 2});

    return world;
}

app_t *app_setup()
{
    app_t *app = malloc(sizeof(app_t));
    app->input = input_create();
    app->shot = NULL;

    app->debug = false;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Milky Way Pool Club");
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

    ball_t *ball = &app->world->balls[0];

    Vector2 shot_vec = shot_vector(app->shot);

    ball_reset_impulses(ball);
    ball_apply_impulse_linear(ball,
                              Vector2Scale(shot_vec, SHOT_IMPULSE_MULTIPLIER));

    app->shot = NULL;
}
