#include "physics/body.h"
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

void addRandomBody(World *world) {
  Vector2 position = {GetRandomValue(0, GetScreenWidth()),
                      GetRandomValue(0, GetScreenHeight())};
  Body *body = bodyCreate(position, 1.0f);
  worldAddBody(world, body);
}

void setupWorld() {
  world = createWorld();

  for (int i = 0; i < 10; i++) {
    addRandomBody(world);
  }
}

void updateWorld(const float dt) {
  const Vector2 mousePosition = GetMousePosition();

  for (int i = 0; i < world->bodiesLength; i++) {
    Body *body = &world->bodies[i];

    const Vector2 position = body->position;
    const Vector2 distance = Vector2Subtract(mousePosition, position);
    const float distanceSquared = Vector2LengthSqr(distance);
    const Vector2 forceDirection = Vector2Normalize(distance);
    const Vector2 force = Vector2Scale(
        forceDirection, G * MOUSE_MASS * body->mass / distanceSquared);

    bodyAddForce(body, force);
  };

  for (int i = 0; i < world->bodiesLength; i++) {
    Body *body = &world->bodies[i];

    bodyIntegrateForces(body, dt);
    bodyIntegrateVelocity(body, dt);
  }

  for (int i = 0; i < world->bodiesLength; i++) {
    Body *body = &world->bodies[i];
    // awful window boundaries
    if (body->position.x < 0) {
      body->position.x = 0;
      body->velocity.x = -body->velocity.x;
    };
    if (body->position.y < 0) {
      body->position.y = 0;
      body->velocity.y = -body->velocity.y;
    };
    if (body->position.x > GetScreenWidth()) {
      body->position.x = GetScreenWidth();
      body->velocity.x = -body->velocity.x;
    };
    if (body->position.y > GetScreenHeight()) {
      body->position.y = GetScreenHeight();
      body->velocity.y = -body->velocity.y;
    };
  };
}

void render() {
  BeginDrawing();

  ClearBackground(BLACK);

  for (int i = 0; i < world->bodiesLength; i++) {
    Body body = world->bodies[i];
    DrawCircle(body.position.x, body.position.y, 10.f, WHITE);
  };

  EndDrawing();
}

int main() {

  setupApp();
  setupWorld();

  while (WindowShouldClose() == false) {
    const float dt = GetFrameTime();
    updateWorld(dt);
    render();
  }

  CloseWindow();

  return 0;
}
