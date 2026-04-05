#ifndef PHYSICS_TABLE_H
#define PHYSICS_TABLE_H

#include <raylib.h>

typedef struct wall {
    Vector2 start;
    Vector2 direction;
    float length;

    float restitution;
    float friction;

    bool is_colliding;
} wall_t;

Vector2 wall_get_outside_normal(const wall_t *wall);

typedef struct hole {
    Vector2 position;
    float radius;
} hole_t;

typedef struct table {
    wall_t *walls;
    int num_walls;
} table_t;

table_t *table_create(const int num_walls,
                      const Vector2 *vertices // clockwise order
);

Vector2 table_get_position(const table_t *table,
                           const Vector2 relative_position);

#endif
