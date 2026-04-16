#include "textures.h"
#define RAYGUI_IMPLEMENTATION

#include "app.h"
#include "physics/shot.h"
#include "raygui.h"
#include "raylib.h"
#include "raymath.h"
#include "render.h"
#include <stdio.h>

#define DEBUG_COLLISION_WALL_COLOR RED
#define DEBUG_NORMAL_WALL_COLOR WHITE

void render_table(const table_t *table, const bool debug)
{
    for (int i = 0; i < ARR_LEN(table->walls); i++) {
        wall_t wall = table->walls[i];
        Color color = (debug && wall.is_colliding) ? DEBUG_COLLISION_WALL_COLOR
                                                   : DEBUG_NORMAL_WALL_COLOR;
        for (int v = 0; v < ARR_LEN(wall.vertices); v++) {
            Vector2 a = wall.vertices[v];
            Vector2 b = wall.vertices[(v + 1) % ARR_LEN(wall.vertices)];
            DrawLine((int) a.x, (int) a.y, (int) b.x, (int) b.y, color);
        }

        if (debug) {
            for (int h = 0; h < ARR_LEN(table->holes); h++) {
                hole_t hole = table->holes[h];
                DrawCircleLines((int) hole.position.x, (int) hole.position.y,
                                (int) hole.radius, color);
            }
        }
    }
}

void render_ball(const ball_t *ball, const textures_t *textures,
                 const bool debug)
{
    Texture2D tex = get_planet_texture(textures, ball->planet);

    Rectangle source = {0, 0, tex.width, tex.height};
    float diameter = ball->radius * 2;
    float tex_ratio =
        tex.width == tex.height ? 1 : (float) tex.width / (float) tex.height;
    Rectangle dest = {ball->position.x, ball->position.y,
                      diameter * fmaxf(tex_ratio, 1),
                      diameter / fminf(tex_ratio, 1)};
    Vector2 origin = {ball->radius * fmaxf(tex_ratio, 1),
                      ball->radius / fminf(tex_ratio, 1)};

    DrawTexturePro(tex, source, dest, origin, ball->rotation * RAD2DEG,
                   (debug && ball->is_colliding) ? RED : WHITE);

    if (debug) {
        DrawCircleLines(ball->position.x, ball->position.y, ball->radius, RED);

        // Ball ID in the center
        char id_text[12];
        snprintf(id_text, sizeof(id_text), "%u", ball->planet);
        const Font font = GetFontDefault();
        const float font_size = 10;
        const float spacing = 2.0f;
        const Vector2 text_size =
            MeasureTextEx(font, id_text, font_size, spacing);
        const Vector2 origin = {text_size.x / 2.0f, text_size.y / 2.0f};
        DrawTextPro(font, id_text, ball->position, origin,
                    ball->rotation * RAD2DEG, font_size, spacing, RED);

        // Rotation line
        const Vector2 rotation_line_start = {
            ball->position.x + 0.4f * ball->radius * cos(ball->rotation),
            ball->position.y + 0.4f * ball->radius * sin(ball->rotation)};
        const Vector2 rotation_line_end = {
            ball->position.x + ball->radius * cos(ball->rotation),
            ball->position.y + ball->radius * sin(ball->rotation)};
        DrawLine(rotation_line_start.x, rotation_line_start.y,
                 rotation_line_end.x, rotation_line_end.y, RED);
    }
}

void render_shot(const ball_t *ball, const shot_t *shot)
{
    Vector2 shot_vec = shot_vector(shot);
    Vector2 shot_dir = Vector2Normalize(shot_vec);
    Vector2 line_start =
        Vector2Add(ball->position, Vector2Scale(shot_dir, ball->radius));

    Vector2 line_end = Vector2Add(line_start, shot_vec);

    DrawLine(line_start.x, line_start.y, line_end.x, line_end.y, WHITE);
}

void render_contact(const contact_t *contact)
{
    DrawCircle(contact->start.x, contact->start.y, 2, RED);
    DrawCircle(contact->end.x, contact->end.y, 2, GREEN);

    DrawLine(contact->start.x, contact->start.y,
             contact->start.x + contact->normal.x * contact->depth,
             contact->start.y + contact->normal.y * contact->depth, RED);
}

void render_debug_info(const world_t *world, const input_t *input)
{
    for (int i = 0; i < world->contacts_length; i++) {
        render_contact(&world->contacts[i]);
    }

    DrawText("Debug Mode ON", 5, GetScreenHeight() - 25, 20, RED);
}

void render_world(const app_t *app)
{
    BeginDrawing();
    ClearBackground(BLACK);

    render_table(app->world->table, app->debug);
    for (int i = 0; i < app->world->balls_length; i++) {
        render_ball(app->world->balls[i], app->textures, app->debug);
    }

    if (app->shot != NULL) {
        render_shot(app->world->balls[0], app->shot);
    }

    if (app->debug) {
        render_debug_info(app->world, app->input);
    }

    EndDrawing();
}
