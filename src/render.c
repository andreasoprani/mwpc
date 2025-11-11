#include "render.h"

void render_world(World *world) {
  BeginDrawing();
  ClearBackground(BLACK);

  render_table(world->table);
  for (int i = 0; i < world->ballsLength; i++) {
    render_ball(&world->balls[i]);
  }

  EndDrawing();
}

void render_table(Table *table) {
  for (int i = 0; i < table->num_walls; i++) {
    Wall *wall = &table->walls[i];
    Wall *next_wall = &table->walls[(i + 1) % table->num_walls];
    Color color = (wall->isColliding) ? RED : WHITE;
    DrawLine(wall->start.x, wall->start.y, next_wall->start.x,
             next_wall->start.y, color);

    Vector2 mid_point = Vector2Lerp(wall->start, next_wall->start, 0.5f);
    Vector2 outside_normal = wall_get_outside_normal(wall);
    DrawLine(mid_point.x, mid_point.y, mid_point.x + outside_normal.x * 10,
             mid_point.y + outside_normal.y * 10, color);
  }
}

void render_ball(Ball *ball) {
  Color color = (ball->isColliding) ? RED : WHITE;
  DrawCircleLines(ball->position.x, ball->position.y, ball->radius, color);
  DrawLine(ball->position.x, ball->position.y,
           ball->position.x + ball->radius * cos(ball->rotation),
           ball->position.y + ball->radius * sin(ball->rotation), color);
}
