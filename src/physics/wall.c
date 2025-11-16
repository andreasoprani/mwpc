#include "wall.h"
#include <raymath.h>
#include <stdlib.h>

const float DEFAULT_WALL_RESTITUTION = 0.9f;
const float DEFAULT_WALL_FRICTION = 0.7f;

Table *table_create(const int num_walls, const Vector2 *vertices) {
  Table *table = malloc(sizeof(Table));
  table->num_walls = num_walls;
  table->walls = malloc(num_walls * sizeof(Wall));
  for (int i = 0; i < num_walls; i++) {
    table->walls[i].start = vertices[i];
    table->walls[i].direction = Vector2Normalize(
        Vector2Subtract(vertices[(i + 1) % num_walls], vertices[i]));

    table->walls[i].restitution = DEFAULT_WALL_RESTITUTION;
    table->walls[i].friction = DEFAULT_WALL_FRICTION;

    // Debug
    table->walls[i].isColliding = false;
  }

  return table;
}

Vector2 wall_get_outside_normal(const Wall *wall) {
  Vector2 normal = {wall->direction.y, -wall->direction.x};
  return normal;
}
