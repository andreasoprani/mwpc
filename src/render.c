#include "render.h"
#include "raylib.h"
#include <stdio.h>

#define DEBUG_BALL_COLOR RED
#define NORMAL_BALL_COLOR WHITE

#define DEBUG_WALL_COLOR RED
#define NORMAL_WALL_COLOR WHITE

void render_table(Table *table, bool debug)
{
    for (int i = 0; i < table->num_walls; i++) {
        Wall *wall = &table->walls[i];
        Wall *next_wall = &table->walls[(i + 1) % table->num_walls];
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

void render_ball(Ball *ball, bool debug)
{
    Color color =
        (debug && ball->is_colliding) ? DEBUG_BALL_COLOR : NORMAL_BALL_COLOR;
    DrawCircleLines(ball->position.x, ball->position.y, ball->radius, color);
    DrawLine(ball->position.x, ball->position.y,
             ball->position.x + ball->radius * cos(ball->rotation),
             ball->position.y + ball->radius * sin(ball->rotation), color);
}

void render_contact(Contact *contact)
{
    DrawCircle(contact->start.x, contact->start.y, 2, RED);
    DrawCircle(contact->end.x, contact->end.y, 2, GREEN);

    DrawLine(contact->start.x, contact->start.y,
             contact->start.x + contact->normal.x * contact->depth,
             contact->start.y + contact->normal.y * contact->depth, RED);
}

void render_world(World *world, bool debug)
{
    BeginDrawing();
    ClearBackground(BLACK);

    render_table(world->table, debug);
    for (int i = 0; i < world->ballsLength; i++) {
        render_ball(&world->balls[i], debug);
    }

    if (debug) {
        for (int i = 0; i < world->contacts_length; i++) {
            render_contact(&world->contacts[i]);
        }
    }

    if (debug) {
        DrawText("Debug Mode ON", 5, GetScreenHeight() - 25, 20, RED);

        for (int i = 0; i < world->ballsLength; i++) {
            char speed_text[128];
            sprintf(speed_text, "Ball %d, position: (%f, %f), speed: (%f, %f)",
                    i, world->balls[i].position.x, world->balls[i].position.y,
                    world->balls[i].velocity.x, world->balls[i].velocity.y);
            DrawText(speed_text, 5, 5 + 20 * i, 20, RED);
        }
    }

    EndDrawing();
}
