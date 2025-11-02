#ifndef WORLD_H
#define WORLD_H

#include "physics/body.h"

typedef struct World {
  Body *bodies;
  int bodiesCapacity;
  int bodiesLength;
} World;

World *createWorld();
void destroyWorld(World *world);

void worldAddBody(World *world, Body *body);

#endif
