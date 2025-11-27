#include "render.h"
#include "raylib.h"

Color DEBUG_BALL_COLOR = WHITE;
Color NORMAL_BALL_COLOR = WHITE;

Color DEBUG_WALL_COLOR = WHITE;
Color NORMAL_WALL_COLOR = WHITE;

void render_world(World *world) {
  BeginDrawing();
  ClearBackground(BLACK);

  render_table(world->table);
  for (int i = 0; i < world->ballsLength; i++) {
    render_ball(&world->balls[i]);
  }

  for (int i = 0; i < world->contactsLength; i++) {
    render_contact(&world->contacts[i]);
  }

  EndDrawing();
}

void render_table(Table *table) {
  for (int i = 0; i < table->num_walls; i++) {
    Wall *wall = &table->walls[i];
    Wall *next_wall = &table->walls[(i + 1) % table->num_walls];
    Color color = (wall->isColliding) ? DEBUG_WALL_COLOR : NORMAL_WALL_COLOR;
    DrawLine(wall->start.x, wall->start.y, next_wall->start.x,
             next_wall->start.y, color);

    Vector2 mid_point = Vector2Lerp(wall->start, next_wall->start, 0.5f);
    Vector2 outside_normal = wall_get_outside_normal(wall);
    DrawLine(mid_point.x, mid_point.y, mid_point.x + outside_normal.x * 10,
             mid_point.y + outside_normal.y * 10, color);
  }
}

void render_ball(Ball *ball) {
  Color color = (ball->isColliding) ? DEBUG_BALL_COLOR : NORMAL_BALL_COLOR;
  DrawCircleLines(ball->position.x, ball->position.y, ball->radius, color);
  DrawLine(ball->position.x, ball->position.y,
           ball->position.x + ball->radius * cos(ball->rotation),
           ball->position.y + ball->radius * sin(ball->rotation), color);
}

void render_contact(Contact *contact) {
  DrawCircle(contact->start.x, contact->start.y, 2, RED);
  DrawCircle(contact->end.x, contact->end.y, 2, GREEN);

  DrawLine(contact->start.x, contact->start.y,
           contact->start.x + contact->normal.x * contact->depth,
           contact->start.y + contact->normal.y * contact->depth, RED);
}
