#ifndef WORLD_H
#define WORLD_H

#include "physics/ball.h"

typedef struct World {
  Ball *balls;
  int ballsCapacity;
  int ballsLength;
} World;

World *world_create();
void world_destroy(World *world);
void world_update(World *world, float dt);

void world_add_ball(World *world, Ball *ball);

#endif
