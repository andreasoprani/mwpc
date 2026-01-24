#include "render.h"
#include "raylib.h"
#include <stdio.h>

#define DEBUG_BALL_COLOR RED
#define NORMAL_BALL_COLOR WHITE

#define DEBUG_WALL_COLOR RED
#define NORMAL_WALL_COLOR WHITE

void render_table(table_t *table, bool debug)
{
    for (int i = 0; i < table->num_walls; i++) {
        wall_t *wall = &table->walls[i];
        wall_t *next_wall = &table->walls[(i + 1) % table->num_walls];
        Color color = (debug && wall->is_colliding) ? DEBUG_WALL_COLOR
                                                    : NORMAL_WALL_COLOR;
        DrawLine(wall->start.x, wall->start.y, next_wall->start.x,
                 next_wall->start.y, color);

        if (debug) {
            Vector2 mid_point =
                Vector2Lerp(wall->start, next_wall->start, 0.5f);
            Vector2 outside_normal = wall_get_outside_normal(wall);
            DrawLine(mid_point.x, mid_point.y,
                     mid_point.x + outside_normal.x * 10,
                     mid_point.y + outside_normal.y * 10, color);
        }
    }
}

void render_ball(ball_t *ball, bool debug)
{
    const Color color =
        (debug && ball->is_colliding) ? DEBUG_BALL_COLOR : NORMAL_BALL_COLOR;

    // Ball outline
    DrawCircleLines(ball->position.x, ball->position.y, ball->radius, color);

    // Ball ID in the center
    char id_text[12];
    snprintf(id_text, sizeof(id_text), "%u", ball->id);
    const Font font = GetFontDefault();
    const float font_size = 10;
    const float spacing = 2.0f;
    const Vector2 text_size = MeasureTextEx(font, id_text, font_size, spacing);
    const Vector2 origin = {text_size.x / 2.0f, text_size.y / 2.0f};
    DrawTextPro(font, id_text, ball->position, origin, ball->rotation * RAD2DEG,
                font_size, spacing, color);

    // Rotation line
    const Vector2 rotation_line_start = {
        ball->position.x + 0.4f * ball->radius * cos(ball->rotation),
        ball->position.y + 0.4f * ball->radius * sin(ball->rotation)};
    const Vector2 rotation_line_end = {
        ball->position.x + ball->radius * cos(ball->rotation),
        ball->position.y + ball->radius * sin(ball->rotation)};
    DrawLine(rotation_line_start.x, rotation_line_start.y, rotation_line_end.x,
             rotation_line_end.y, color);
}

void render_contact(contact_t *contact)
{
    DrawCircle(contact->start.x, contact->start.y, 2, RED);
    DrawCircle(contact->end.x, contact->end.y, 2, GREEN);

    DrawLine(contact->start.x, contact->start.y,
             contact->start.x + contact->normal.x * contact->depth,
             contact->start.y + contact->normal.y * contact->depth, RED);
}

void render_world(world_t *world, bool debug)
{
    BeginDrawing();
    ClearBackground(BLACK);

    render_table(world->table, debug);
    for (int i = 0; i < world->balls_length; i++) {
        render_ball(&world->balls[i], debug);
    }

    if (debug) {
        for (int i = 0; i < world->contacts_length; i++) {
            render_contact(&world->contacts[i]);
        }
    }

    if (debug) {
        DrawText("Debug Mode ON", 5, GetScreenHeight() - 25, 20, RED);

        for (int i = 0; i < world->balls_length; i++) {
            char speed_text[128];
            snprintf(speed_text, sizeof(speed_text),
                     "ball_t %d, position: (%f, %f), speed: "
                     "(%f, %f)",
                     i, world->balls[i].position.x, world->balls[i].position.y,
                     world->balls[i].velocity.x, world->balls[i].velocity.y);
            DrawText(speed_text, 5, 5 + 20 * i, 20, RED);
        }
    }

    EndDrawing();
}
