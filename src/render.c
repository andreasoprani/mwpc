#include "physics/constants.h"
#include "physics/planets.h"
#include "physics/table.h"
#include "textures.h"

#include "app.h"
#include "constants.h"
#include "raylib.h"
#include "raymath.h"
#include "render.h"
#include "shot.h"
#include <stdio.h>
#include <string.h>

typedef struct render_transform {
    Vector2 offset;
    float scale;
} render_transform_t;

static render_transform_t render_get_transform(const world_t *world)
{
    const float padding = GetScreenHeight() * TABLE_H_PAD_RATIO;
    const float available_width =
        fmaxf(1.0f, GetScreenWidth() - 2.0f * padding);
    const float available_height =
        fmaxf(1.0f, GetScreenHeight() - 2.0f * padding);

    (void) world;
    const float scale =
        fminf(available_width / TABLE_WIDTH, available_height / TABLE_HEIGHT);

    const Vector2 table_screen_size = {TABLE_WIDTH * scale,
                                       TABLE_HEIGHT * scale};
    const Vector2 table_screen_origin = {
        (GetScreenWidth() - table_screen_size.x) / 2.0f,
        (GetScreenHeight() - table_screen_size.y) / 2.0f,
    };

    return (render_transform_t) {
        .offset = table_screen_origin,
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
            DrawLine((int) a.x, (int) a.y, (int) b.x, (int) b.y, WHITE);
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
    { // Display shot start circles
        Vector2 shot_vec = shot_vector(shot);
        float shot_length = Vector2Length(shot_vec);
        Vector2 circle_center = world_to_screen(transform, shot->start);
        unsigned int circles = (int) (shot_length / (SHOT_MAX_LENGTH / 3)) + 1;
        float circle_radius;
        for (int i = 0; i < circles; i++) {
            circle_radius = SHOT_MAX_RADIUS_HEIGHT_FRACTION *
                            GetScreenHeight() *
                            fminf((i + 1) * SHOT_MAX_LENGTH / 3, shot_length) /
                            SHOT_MAX_LENGTH;
            DrawCircleLines(circle_center.x, circle_center.y, circle_radius,
                            WHITE);
        }

        Vector2 line_end = Vector2Subtract(
            circle_center,
            Vector2Scale(Vector2Normalize(shot_vec), circle_radius));

        DrawLine(circle_center.x, circle_center.y, line_end.x, line_end.y,
                 WHITE);
    }

    { // Display trajectories
        for (int t = 0; t < ARR_LEN(shot->trajectories); t++) {
            const trajectory_t *trajectory = &shot->trajectories[t];
            const Color color = trajectory->planet == EARTH ? RED : WHITE;
            for (int i = 0; i < trajectory->positions_length; i++) {
                Vector2 position =
                    world_to_screen(transform, trajectory->positions[i]);
                DrawCircle(position.x, position.y, 1, color);
            }
        }
    }
}

void render_score(const unsigned int shots_count)
{
    const float score_font_size = 20;
    char score_text[32];
    snprintf(score_text, sizeof(score_text), "Shots: %u", shots_count);
    DrawText(score_text, 5, 5, score_font_size, WHITE);
}

static void draw_text_centered(const char *text, const float center_x,
                               const float y, const float font_size,
                               const float spacing, const Color color)
{
    char line[256];
    int line_start = 0;
    int line_length = 0;
    float line_y = y;

    for (int i = 0;; i++) {
        if (text[i] != '\n' && text[i] != '\0') {
            line_length++;
            continue;
        }

        memcpy(line, text + line_start, line_length);
        line[line_length] = '\0';

        Vector2 line_size =
            MeasureTextEx(GetFontDefault(), line, font_size, spacing);
        DrawTextEx(GetFontDefault(), line,
                   (Vector2) {center_x - line_size.x / 2.0f, line_y},
                   font_size, spacing, color);

        if (text[i] == '\0')
            break;

        line_start = i + 1;
        line_length = 0;
        line_y += line_size.y;
    }
}

void render_app_state(const app_state_t state, const unsigned int shots_count,
                      const render_transform_t transform)
{
    if (state == APP_STATE_RUNNING)
        return;

    char *title = "";
    char *body = "";
    char shots_text[32];
    bool display_shots = false;
    switch (state) {
    case APP_STATE_MENU:
        title = "Welcome to\nMilky Way Pool Club!";
        body = "[SPACE] / [TAP]: start\n"
               "[C]: credits\n"
               "\nIn game:\n"
               "[LCLICK]/touch: aim\n"
               "Release: shoot\n"
               "[ESC]: pause";
        break;
    case APP_STATE_RUNNING:
        break;
    case APP_STATE_PAUSED:
        title = "PAUSE";
        body = "[SPACE] / [TAP]: resume\n"
               "\nIn game:\n"
               "[LCLICK]/touch: aim\n"
               "Release: shoot\n"
               "[ESC]: pause";
        break;
    case APP_STATE_WIN:
        title = "YOU WIN";
        body = "[SPACE] / [TAP]: play again\n"
               "[C]: credits\n"
               "\nIn game:\n"
               "[LCLICK]/touch: aim\n"
               "Release: shoot\n"
               "[ESC]: pause";
        snprintf(shots_text, sizeof(shots_text), "Shots: %u", shots_count);
        display_shots = true;
        break;
    case APP_STATE_LOSE:
        title = "GAME OVER";
        body = "[SPACE] / [TAP]: play again\n"
               "[C]: credits\n"
               "\nIn game:\n"
               "[LCLICK]/touch: aim\n"
               "Release: shoot\n"
               "[ESC]: pause";
        snprintf(shots_text, sizeof(shots_text), "Shots: %u", shots_count);
        display_shots = true;
        break;
    case APP_STATE_CREDITS_PAUSED:
    case APP_STATE_CREDITS_MAIN:
        title = "CREDITS";
        body = "Made by Andrea Soprani (sprn.it)\n"
               "Planet sprites by localthunk (Balatro)\n"
               "Physics system inspired by Pikuma\n"
               "(pikuma.com/courses/game-physics-engine-programming)\n\n"
               "[SPACE] / [ESC] / [TAP]: back";
        break;
    }

    const float screen_min = fminf(GetScreenWidth(), GetScreenHeight());
    const float table_min =
        fminf(TABLE_WIDTH * transform.scale, TABLE_HEIGHT * transform.scale);
    const float ui_scale = fminf(screen_min, table_min);

    const float main_font_size = fmaxf(20.0f, ui_scale * 0.05f);
    const float keys_font_size = fmaxf(14.0f, ui_scale * 0.035f);
    const float main_spacing = fmaxf(1.0f, ui_scale * 0.004f);
    const float body_spacing = fmaxf(1.0f, ui_scale * 0.003f);

    const Vector2 center =
        (Vector2) {GetScreenWidth() / 2, GetScreenHeight() / 2};

    Vector2 title_size =
        MeasureTextEx(GetFontDefault(), title, main_font_size, main_spacing);
    Vector2 body_size =
        MeasureTextEx(GetFontDefault(), body, keys_font_size, body_spacing);
    Vector2 shots_text_size = display_shots
                                  ? MeasureTextEx(GetFontDefault(), shots_text,
                                                  keys_font_size, body_spacing)
                                  : (Vector2) {0, 0};

    const float padding = fmaxf(14.0f, ui_scale * 0.05f);

    const float max_rect_width = TABLE_WIDTH * transform.scale * 0.95f;
    const float max_rect_height = TABLE_HEIGHT * transform.scale * 0.95f;

    float rect_width =
        fmaxf(title_size.x, fmaxf(body_size.x, shots_text_size.x)) +
        2 * padding;
    float rect_height = title_size.y + body_size.y +
                        (display_shots ? (shots_text_size.y + padding) : 0) +
                        3 * padding;

    rect_width = fminf(rect_width, max_rect_width);
    rect_height = fminf(rect_height, max_rect_height);
    float rect_outline = 2;

    DrawRectangle(center.x - (rect_width + rect_outline) / 2,
                  center.y - (rect_height + rect_outline) / 2,
                  (rect_width + rect_outline), (rect_height + rect_outline),
                  WHITE);
    DrawRectangle(center.x - rect_width / 2, center.y - rect_height / 2,
                  rect_width, rect_height, BLACK);

    draw_text_centered(title, center.x, center.y - rect_height / 2 + padding,
                       main_font_size, main_spacing, WHITE);

    if (display_shots)
        DrawTextEx(
            GetFontDefault(), shots_text,
            (Vector2) {center.x - shots_text_size.x / 2,
                       center.y - rect_height / 2 + 2 * padding + title_size.y},
            keys_font_size, body_spacing, WHITE);

    DrawTextEx(
        GetFontDefault(), body,
        (Vector2) {center.x - body_size.x / 2,
                   center.y - rect_height / 2 + 2 * padding + title_size.y +
                       (display_shots ? (padding + shots_text_size.y) : 0)},
        keys_font_size, body_spacing, WHITE);
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

    render_app_state(app->state, app->shots_count, transform);

    EndDrawing();
}
