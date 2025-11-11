#include "physics/ball.h"
#include "physics/wall.h"
#include "raylib.h"
#include "render.h"
#include "world.h"
#include <stdio.h>

const float G = 9.81f;
const float MOUSE_MASS = 100000;

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

void update_world(const float dt) {
  const Vector2 mousePosition = GetMousePosition();

  world_update(world, dt);

  for (int i = 0; i < world->ballsLength; i++) {
    Ball *ball = &world->balls[i];
    // awful window boundaries
    if (ball->position.x < 0) {
      ball->position.x = 0;
      ball->velocity.x = -.9 * ball->velocity.x;
    };
    if (ball->position.y < 0) {
      ball->position.y = 0;
      ball->velocity.y = -.9 * ball->velocity.y;
    };
    if (ball->position.x > GetScreenWidth()) {
      ball->position.x = GetScreenWidth();
      ball->velocity.x = -.9 * ball->velocity.x;
    };
    if (ball->position.y > GetScreenHeight()) {
      ball->position.y = GetScreenHeight();
      ball->velocity.y = -.9 * ball->velocity.y;
    };
  };
}

int main() {

  setupApp();
  setupWorld();

  while (WindowShouldClose() == false) {
    const float dt = GetFrameTime();

    input();

    update_world(dt);
    render_world(world);
  }

  CloseWindow();

  return 0;
}
