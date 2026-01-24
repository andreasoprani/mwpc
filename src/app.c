#include "app.h"
#include "input.h"
#include "raylib.h"
#include "render.h"
#include "world.h"
#include <stdio.h>
#include <stdlib.h>

const float SCREEN_WIDTH = 900;
const float SCREEN_HEIGHT = 900;

void toggle_debug(App *app)
{
    const bool curr_debug = app->debug;
    app->debug = !curr_debug;
    app->debugRun = curr_debug;
    printf("Debug mode: %s\n", app->debug ? "ON" : "OFF");
}

Ball *add_ball(World *world, Vector2 position)
{
    Ball *ball = ball_create(position, 20.f, 100.f);
    ball->velocity.x = 1000.0f;
    world_add_ball(world, ball);
    return ball;
}

World *world_setup(App *app)
{
    int wall_padding = 50;

    Vector2 tl = {wall_padding, wall_padding};
    Vector2 tr = {GetScreenWidth() - wall_padding, wall_padding};
    Vector2 bl = {wall_padding, GetScreenHeight() - wall_padding};
    Vector2 br = {GetScreenWidth() - wall_padding,
                  GetScreenHeight() - wall_padding};

    Vector2 vertices[4] = {tl, tr, br, bl};

    Table *table = table_create(4, vertices);

    World *world = world_create(table);

    add_ball(world, (Vector2) {GetScreenWidth() / 2, GetScreenHeight() / 2});

    return world;
}

App *app_setup()
{
    App *app = malloc(sizeof(App));
    app->input = input_create();

    app->debug = false;
    app->debugRun = true;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Milky Way Pool Club");
    SetTargetFPS(120);
    app->world = world_setup(app);
    return app;
}

void apply_app_inputs(App *app)
{
    if (app->input->keyGPressed)
        world_toggle_gravity(app->world);

    if (app->input->keyDPressed)
        toggle_debug(app);

    if (app->debug && app->input->keySpacePressed) {
        app->debugRun = true;
    }

    if (app->input->mouseLeftPressed) {
        add_ball(app->world, app->input->mousePosition);
    }
}

void app_frame(App *app)
{
    float dt = GetFrameTime();

    input_update(app->input);
    apply_app_inputs(app);

    if (!app->debug || app->debugRun) {
        world_update(app->world, dt);
        app->debugRun = !app->debug;
    }
    render_world(app->world, app->debug);
}
