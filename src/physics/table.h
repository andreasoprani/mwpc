#ifndef PHYSICS_TABLE_H
#define PHYSICS_TABLE_H

#include <raylib.h>

typedef struct wall {
    Vector2 vertices[4]; // world-space, clockwise order

    float restitution;
    float friction;

    bool is_colliding;
} wall_t;

typedef struct hole {
    Vector2 position;
    float radius;
    float rotation;
} hole_t;

#define NUM_WALLS 6

typedef struct table {
    wall_t walls[NUM_WALLS];

    hole_t holes[6]; // clock-wise from top-left

} table_t;

void table_setup(table_t *table);

#endif
