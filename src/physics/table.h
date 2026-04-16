#ifndef PHYSICS_TABLE_H
#define PHYSICS_TABLE_H

#include <raylib.h>

typedef struct wall {
    // TODO: better shape?
    Vector2 vertices[6]; // world-space, clockwise order

    float restitution;
    float friction;

    bool is_colliding;
} wall_t;

typedef struct hole {
    Vector2 position;
    float radius;
} hole_t;

typedef struct table {
    wall_t walls[6];

    hole_t holes[6]; // clock-wise from top-left

    Vector2 origin; // top-left corner of the playing area
    float width;
    float height;
} table_t;

table_t *table_create(Vector2 origin, float width, float height);

Vector2 table_get_position(const table_t *table,
                           const Vector2 relative_position);

#endif
