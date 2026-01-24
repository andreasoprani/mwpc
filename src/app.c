#include "app.h"
#include "input.h"
#include "raylib.h"
#include "render.h"
#include "world.h"
#include <stdio.h>
#include <stdlib.h>

const float SCREEN_WIDTH = 900;
const float SCREEN_HEIGHT = 900;

void toggle_debug(app_t *app)
{
    const bool curr_debug = app->debug;
    app->debug = !curr_debug;
    printf("Debug mode: %s\n", app->debug ? "ON" : "OFF");
}

ball_t *add_ball(world_t *world, Vector2 position)
{
    ball_t *ball = ball_create(position, 20.f, 100.f);
    ball->velocity.x = 1000.0f;
    world_add_ball(world, ball);
    return ball;
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

    add_ball(world, (Vector2) {GetScreenWidth() / 2, GetScreenHeight() / 2});

    return world;
}

app_t *app_setup()
{
    app_t *app = malloc(sizeof(app_t));
    app->input = input_create();

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

    if (app->input->mouse_left_pressed) {
        add_ball(app->world, app->input->mouse_position);
    }
}

void app_frame(app_t *app)
{
    float dt = GetFrameTime();

    input_update(app->input);
    apply_app_inputs(app);

    if (!app->debug || app->input->key_space_pressed) {
        world_update(app->world, dt);
    }
    render_world(app->world, app->debug);
}
