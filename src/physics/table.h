#ifndef PHYSICS_TABLE_H
#define PHYSICS_TABLE_H

#include <raylib.h>

typedef struct wall {
    Vector2 *vertices; // world-space, clockwise order
    int num_vertices;

    float restitution;
    float friction;

    bool is_colliding;
} wall_t;

typedef struct hole {
    Vector2 position;
    float radius;
} hole_t;

typedef struct table {
    wall_t *walls;
    int num_walls;

    Vector2 origin; // top-left corner of the playing area
    float width;
    float height;
} table_t;

table_t *table_create(Vector2 origin, float width, float height);

Vector2 table_get_position(const table_t *table,
                           const Vector2 relative_position);

#endif
