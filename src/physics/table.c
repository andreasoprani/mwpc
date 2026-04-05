#include "table.h"
#include "constants.h"
#include "raymath.h"
#include <stdlib.h>

table_t *table_create(Vector2 origin, float width, float height)
{
    table_t *table = malloc(sizeof(table_t));
    table->origin = origin;
    table->width = width;
    table->height = height;

    const Vector2 vertices[4] = {
        {origin.x, origin.y},
        {origin.x + width, origin.y},
        {origin.x + width, origin.y + height},
        {origin.x, origin.y + height},
    };

    const int num_walls = 4;
    table->num_walls = num_walls;
    table->walls = malloc(num_walls * sizeof(wall_t));

    for (int i = 0; i < num_walls; i++) {
        Vector2 v0 = vertices[i];
        Vector2 v1 = vertices[(i + 1) % num_walls];

        Vector2 edge_dir = Vector2Normalize(Vector2Subtract(v1, v0));
        Vector2 outward = (Vector2) {edge_dir.y, -edge_dir.x};

        wall_t *wall = &table->walls[i];
        wall->num_vertices = 4;
        wall->vertices = malloc(4 * sizeof(Vector2));
        wall->vertices[0] =
            Vector2Add(v0, Vector2Scale(outward, WALL_THICKNESS));
        wall->vertices[1] =
            Vector2Add(v1, Vector2Scale(outward, WALL_THICKNESS));
        wall->vertices[2] = v1;
        wall->vertices[3] = v0;

        wall->restitution = DEFAULT_WALL_RESTITUTION;
        wall->friction = DEFAULT_WALL_FRICTION;
        wall->is_colliding = false;
    }

    return table;
}

Vector2 table_get_position(const table_t *table,
                           const Vector2 relative_position)
{
    return (Vector2) {table->origin.x + relative_position.x * table->width,
                      table->origin.y + relative_position.y * table->height};
}
