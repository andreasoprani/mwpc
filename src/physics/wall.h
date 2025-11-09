#ifndef PHYSICS_WALL_H
#define PHYSICS_WALL_H

#include <raylib.h>

typedef struct Wall {
  Vector2 position;
  Vector2 size;
} Wall;

Wall *create_wall(Vector2 position, Vector2 size);

#endif
