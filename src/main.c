#include "physics/ball.h"
#include "physics/wall.h"
#include "raylib.h"
#include "render.h"
#include "world.h"
#include <stdio.h>

const float SCREEN_WIDTH = 900;
const float SCREEN_HEIGHT = 900;

static World *world = NULL;

void setupApp() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Milky Way Pool Club");
  SetTargetFPS(120);
}

void add_random_ball(World *world) {
  Vector2 position = {GetRandomValue(0, GetScreenWidth()),
                      GetRandomValue(0, GetScreenHeight())};
  Ball *ball = ball_create(position, 20.f, 100.f);
  world_add_ball(world, ball);
}

void add_ball(World *world, Vector2 position) {
  Ball *ball = ball_create(position, 20.f, 100.f);
  world_add_ball(world, ball);
}

void setupWorld() {

  int wall_padding = 50;

  Vector2 tl = {wall_padding, wall_padding};
  Vector2 tr = {GetScreenWidth() - wall_padding, wall_padding};
  Vector2 bl = {wall_padding, GetScreenHeight() - wall_padding};
  Vector2 br = {GetScreenWidth() - wall_padding,
                GetScreenHeight() - wall_padding};

  Vector2 vertices[4] = {tl, tr, br, bl};

  Table *table = table_create(4, vertices);

  world = world_create(table);
}

void input() {
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    add_ball(world, GetMousePosition());
  }
}

int main() {

  setupApp();
  setupWorld();

  while (WindowShouldClose() == false) {
    float dt = GetFrameTime();

    input();

    world_update(world, dt);

    render_world(world);
  }

  CloseWindow();

  return 0;
}
