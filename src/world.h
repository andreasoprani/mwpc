#ifndef WORLD_H
#define WORLD_H

#include "physics/ball.h"
#include "physics/wall.h"

typedef struct World {
  Ball *balls;
  int ballsCapacity;
  int ballsLength;

  Table *table;
} World;

World *world_create(Table *table);
void world_destroy(World *world);
void world_update(World *world, float dt);

void world_add_ball(World *world, Ball *ball);

#endif
