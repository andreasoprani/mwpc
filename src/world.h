#ifndef WORLD_H
#define WORLD_H

#include "physics/ball.h"
#include "physics/contact.h"
#include "physics/wall.h"

typedef struct World {
  Ball *balls;
  unsigned int ballsCapacity;
  unsigned int ballsLength;

  Table *table;

  Contact *contacts;
  unsigned int contactsCapacity;
  unsigned int contactsLength;

  bool gravityEnabled;
} World;

World *world_create(Table *table);
void world_destroy(World *world);
void world_update(World *world, float dt);

void world_add_ball(World *world, Ball *ball);

void worldToggleGravity(World *world);

#endif
