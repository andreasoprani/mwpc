#include "physics/constants.h"
#include "physics/table.h"
#include "textures.h"

#include "app.h"
#include "constants.h"
#include "physics/shot.h"
#include "raylib.h"
#include "raymath.h"
#include "render.h"
#include <stdio.h>
#include <string.h>

#define WALL_COLOR WHITE
#define SHOT_START_CIRCLE_RADIUS 10

typedef struct render_transform {
    Vector2 offset;
    float scale;
} render_transform_t;

static render_transform_t render_get_transform(const world_t *world)
{
    const float padding = TABLE_H_PAD;
    const float available_width =
        fmaxf(1.0f, GetScreenWidth() - 2.0f * padding);
    const float available_height =
        fmaxf(1.0f, GetScreenHeight() - 2.0f * padding);

    const float scale = fminf(available_width / world->table.width,
                              available_height / world->table.height);

    const Vector2 table_screen_size = {world->table.width * scale,
                                       world->table.height * scale};
    const Vector2 table_screen_origin = {
        (GetScreenWidth() - table_screen_size.x) / 2.0f,
        (GetScreenHeight() - table_screen_size.y) / 2.0f,
    };

    return (render_transform_t) {
        .offset = Vector2Subtract(table_screen_origin,
                                  Vector2Scale(world->table.origin, scale)),
        .scale = scale,
    };
}

static Vector2 world_to_screen(render_transform_t transform,
                               const Vector2 position)
{
    return Vector2Add(transform.offset,
                      Vector2Scale(position, transform.scale));
}

Vector2 render_screen_to_world(const world_t *world, Vector2 screen_position)
{
    const render_transform_t transform = render_get_transform(world);
    return Vector2Scale(Vector2Subtract(screen_position, transform.offset),
                        1.0f / transform.scale);
}

void draw_circle_texture(const Texture2D tex, const Vector2 position,
                         const float radius, const float rotation)
{
    Rectangle source = {0, 0, tex.width, tex.height};
    float diameter = radius * 2.0f;
    float tex_ratio =
        tex.width == tex.height ? 1 : (float) tex.width / (float) tex.height;
    Rectangle dest = {position.x, position.y, diameter * fmaxf(tex_ratio, 1),
                      diameter / fminf(tex_ratio, 1)};
    Vector2 origin = {radius * fmaxf(tex_ratio, 1),
                      radius / fminf(tex_ratio, 1)};

    DrawTexturePro(tex, source, dest, origin, rotation * RAD2DEG, WHITE);
}

void render_table(const table_t table, const textures_t *textures,
                  const render_transform_t transform)
{
    for (int h = 0; h < ARR_LEN(table.holes); h++) {
        hole_t hole = table.holes[h];

        draw_circle_texture(textures->hole,
                            world_to_screen(transform, hole.position),
                            hole.radius * transform.scale, hole.rotation);
    }

    for (int i = 0; i < ARR_LEN(table.walls); i++) {
        wall_t wall = table.walls[i];
        for (int v = 0; v < ARR_LEN(wall.vertices); v++) {
            Vector2 a = world_to_screen(transform, wall.vertices[v]);
            Vector2 b = world_to_screen(
                transform, wall.vertices[(v + 1) % ARR_LEN(wall.vertices)]);
            DrawLine((int) a.x, (int) a.y, (int) b.x, (int) b.y, WALL_COLOR);
        }
    }
}

void render_ball(const ball_t *ball, const textures_t *textures,
                 const render_transform_t transform)
{
    Texture2D tex = get_planet_texture(textures, ball->planet);
    draw_circle_texture(tex, world_to_screen(transform, ball->position),
                        ball->radius * transform.scale, ball->rotation);
}

void render_shot(const ball_t *ball, const shot_t *shot,
                 const render_transform_t transform)
{
    { // Display shot start circle
        Vector2 start_screen = world_to_screen(transform, shot->start);
        DrawCircleLines(start_screen.x, start_screen.y,
                        SHOT_START_CIRCLE_RADIUS, WHITE);

        Vector2 shot_vec = shot_vector(shot);
        Vector2 line_end = Vector2Subtract(shot->start, shot_vec);
        line_end = world_to_screen(transform, line_end);
        DrawLine(start_screen.x, start_screen.y, line_end.x, line_end.y, WHITE);
    }

    { // Display trajectory (TODO: remove when trajectories are implemented)
        Vector2 shot_vec = shot_vector(shot);
        Vector2 shot_dir = Vector2Normalize(shot_vec);
        Vector2 line_start =
            Vector2Add(ball->position, Vector2Scale(shot_dir, ball->radius));

        Vector2 line_end = Vector2Add(line_start, shot_vec);

        line_start = world_to_screen(transform, line_start);
        line_end = world_to_screen(transform, line_end);

        DrawLine(line_start.x, line_start.y, line_end.x, line_end.y, WHITE);
    }
}

void render_score(const unsigned int shots_count)
{
    const float score_font_size = 20;
    char score_text[32];
    snprintf(score_text, sizeof(score_text), "Shots: %u", shots_count);
    DrawText(score_text, 5, 5, score_font_size, WHITE);
}

void render_app_state(const app_state_t state, const unsigned int shots_count)
{
    if (state == APP_STATE_RUNNING)
        return;

    char *main_text = "";
    char *run_text = "";
    char shots_text[32];
    bool display_shots = false;
    switch (state) {
    case APP_STATE_MENU:
        main_text = "Welcome to Milky Way Pool Club!";
        run_text = "Press SPACE to start";
        break;
    case APP_STATE_RUNNING:
        break;
    case APP_STATE_PAUSED:
        main_text = "PAUSE";
        run_text = "Press SPACE to resume";
        break;
    case APP_STATE_WIN:
        main_text = "YOU WIN";
        run_text = "Press SPACE to restart";
        snprintf(shots_text, sizeof(shots_text), "Shots: %u", shots_count);
        display_shots = true;
        break;
    case APP_STATE_LOSE:
        main_text = "GAME OVER";
        run_text = "Press SPACE to restart";
        snprintf(shots_text, sizeof(shots_text), "Shots: %u", shots_count);
        display_shots = true;
        break;
    }

    const float main_font_size = 30;
    const float keys_font_size = 20;
    const float main_spacing = 3;
    const float small_texts_spacing = 2;
    const float v_small_texts_spacing = display_shots ? 10 : 0;

    const Vector2 center =
        (Vector2) {GetScreenWidth() / 2, GetScreenHeight() / 2};

    Vector2 main_text_size = MeasureTextEx(GetFontDefault(), main_text,
                                           main_font_size, main_spacing);
    Vector2 run_text_size = MeasureTextEx(GetFontDefault(), run_text,
                                          keys_font_size, small_texts_spacing);
    Vector2 shots_text_size = MeasureTextEx(
        GetFontDefault(), shots_text, keys_font_size, small_texts_spacing);

    const float padding = 50;

    float rw =
        fmaxf(main_text_size.x, fmaxf(run_text_size.x, shots_text_size.x)) +
        2 * padding;
    float rh =
        main_text_size.y + run_text_size.y +
        (display_shots ? (shots_text_size.y + v_small_texts_spacing) : 0) +
        3 * padding;
    float r_outline = 2;

    DrawRectangle(center.x - (rw + r_outline) / 2,
                  center.y - (rh + r_outline) / 2, (rw + r_outline),
                  (rh + r_outline), WHITE);
    DrawRectangle(center.x - rw / 2, center.y - rh / 2, rw, rh, BLACK);

    DrawTextEx(GetFontDefault(), main_text,
               (Vector2) {center.x - main_text_size.x / 2,
                          center.y - rh / 2 + padding},
               main_font_size, main_spacing, WHITE);

    DrawTextEx(GetFontDefault(), run_text,
               (Vector2) {center.x - run_text_size.x / 2,
                          center.y - rh / 2 + 2 * padding + main_text_size.y},
               keys_font_size, small_texts_spacing, WHITE);

    if (display_shots)
        DrawTextEx(GetFontDefault(), shots_text,
                   (Vector2) {center.x - shots_text_size.x / 2,
                              center.y - rh / 2 + 2 * padding +
                                  main_text_size.y + v_small_texts_spacing +
                                  run_text_size.y},
                   keys_font_size, small_texts_spacing, WHITE);
}

void render(const app_t *app)
{
    const render_transform_t transform = render_get_transform(app->world);

    BeginDrawing();
    ClearBackground(BLACK);

    render_table(app->world->table, &app->textures, transform);
    for (int i = 0; i < app->world->balls_count; i++) {
        render_ball(&app->world->balls[i], &app->textures, transform);
    }

    if (app->shot != NULL) {
        render_shot(&app->world->balls[0], app->shot, transform);
    }

    render_score(app->shots_count);

    render_app_state(app->state, app->shots_count);

    EndDrawing();
}
