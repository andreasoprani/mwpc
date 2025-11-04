#include "physics/ball.h"
#include "raylib.h"
#include "raymath.h"
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

void setupWorld() {
  world = world_create();

  for (int i = 0; i < 20; i++) {
    add_random_ball(world);
  }
}

void updateWorld(const float dt) {
  const Vector2 mousePosition = GetMousePosition();

  // for (int i = 0; i < world->ballsLength; i++) {
  //   Ball *ball = &world->balls[i];

  //   const Vector2 position = ball->position;
  //   const Vector2 distance = Vector2Subtract(mousePosition, position);
  //   const float distanceSquared = Vector2LengthSqr(distance);
  //   const Vector2 forceDirection = Vector2Normalize(distance);
  //   const Vector2 force = Vector2Scale(
  //       forceDirection, G * MOUSE_MASS * ball->mass / distanceSquared);

  //   ball_add_force(ball, force);
  // };

  world_update(world, dt);

  for (int i = 0; i < world->ballsLength; i++) {
    Ball *ball = &world->balls[i];
    // awful window boundaries
    if (ball->position.x < 0) {
      ball->position.x = 0;
      ball->velocity.x = -ball->velocity.x;
    };
    if (ball->position.y < 0) {
      ball->position.y = 0;
      ball->velocity.y = -ball->velocity.y;
    };
    if (ball->position.x > GetScreenWidth()) {
      ball->position.x = GetScreenWidth();
      ball->velocity.x = -ball->velocity.x;
    };
    if (ball->position.y > GetScreenHeight()) {
      ball->position.y = GetScreenHeight();
      ball->velocity.y = -ball->velocity.y;
    };
  };
}

void render() {
  for (int i = 0; i < world->ballsLength; i++) {
    Ball *ball = &world->balls[i];
    DrawCircleLines(ball->position.x, ball->position.y, ball->radius, WHITE);
  };
}

int main() {

  setupApp();
  setupWorld();

  while (WindowShouldClose() == false) {
    const float dt = GetFrameTime();

    // Move to rendering
    BeginDrawing();
    ClearBackground(BLACK);

    updateWorld(dt);
    render();

    // Move to rendering
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
