#ifndef PHYSICS_WALL_H
#define PHYSICS_WALL_H

#include <raylib.h>

typedef struct Wall {
  Vector2 start;
  Vector2 direction;

  float restitution;
  float friction;

  bool isColliding;
} Wall;

Vector2 wall_get_outside_normal(const Wall *wall);

typedef struct Table {
  Wall *walls;
  int num_walls;
} Table;

Table *table_create(const int num_walls,
                    const Vector2 *vertices // clockwise order
);

#endif
