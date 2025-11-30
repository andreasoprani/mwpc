#include "app.h"
#include "input.h"
#include "raylib.h"
#include "render.h"
#include "world.h"
#include <stdio.h>
#include <stdlib.h>

const float SCREEN_WIDTH = 900;
const float SCREEN_HEIGHT = 900;

void toggleDebug(App *app) {
  const bool currDebug = app->debug;
  app->debug = !currDebug;
  app->debugRun = currDebug;
  printf("Debug mode: %s\n", app->debug ? "ON" : "OFF");
}

Ball *addBall(World *world, Vector2 position) {
  Ball *ball = ball_create(position, 20.f, 100.f);
  world_add_ball(world, ball);
  return ball;
}

World *worldSetup(App *app) {
  int wall_padding = 50;

  Vector2 tl = {wall_padding, wall_padding};
  Vector2 tr = {GetScreenWidth() - wall_padding, wall_padding};
  Vector2 bl = {wall_padding, GetScreenHeight() - wall_padding};
  Vector2 br = {GetScreenWidth() - wall_padding,
                GetScreenHeight() - wall_padding};

  Vector2 vertices[4] = {tl, tr, br, bl};

  Table *table = table_create(4, vertices);

  World *world = world_create(table);

  Ball *ball_1 =
      addBall(world, (Vector2){GetScreenWidth() / 2, GetScreenHeight() / 4});
  Ball *ball_2 =
      addBall(world, (Vector2){GetScreenWidth() / 2, GetScreenHeight() / 2});
  Ball *ball_3 = addBall(
      world, (Vector2){GetScreenWidth() / 2, 3 * GetScreenHeight() / 4});

  ball_1->velocity = (Vector2){100.0f, 0.0f};
  ball_2->velocity = (Vector2){100.0f, 0.0f};
  ball_3->velocity = (Vector2){100.0f, 0.0f};

  return world;
}

App *appSetup() {
  App *app = malloc(sizeof(App));
  app->input = inputCreate();

  app->debug = false;
  app->debugRun = true;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Milky Way Pool Club");
  // SetTargetFPS(120);
  app->world = worldSetup(app);
  return app;
}

void applyAppInputs(App *app) {

  if (app->input->keyGPressed)
    worldToggleGravity(app->world);

  if (app->input->keyDPressed)
    toggleDebug(app);

  if (app->debug && app->input->keySpacePressed) {
    app->debugRun = true;
  }

  if (app->input->mouseLeftPressed) {
    addBall(app->world, app->input->mousePosition);
  }
}

void appFrame(App *app) {
  float dt = GetFrameTime();

  inputUpdate(app->input);
  applyAppInputs(app);

  if (!app->debug || app->debugRun) {
    world_update(app->world, dt);
    app->debugRun = !app->debug;
  }
  renderWorld(app->world, app->debug);
}
