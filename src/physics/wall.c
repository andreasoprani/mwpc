#include "wall.h"
#include "constants.h"
#include <raymath.h>
#include <stdlib.h>

table_t *table_create(const int num_walls, const Vector2 *vertices)
{
    table_t *table = malloc(sizeof(table_t));
    table->num_walls = num_walls;
    table->walls = malloc(num_walls * sizeof(wall_t));
    for (int i = 0; i < num_walls; i++) {
        table->walls[i].start = vertices[i];
        table->walls[i].direction = Vector2Normalize(
            Vector2Subtract(vertices[(i + 1) % num_walls], vertices[i]));

        table->walls[i].restitution = DEFAULT_WALL_RESTITUTION;
        table->walls[i].friction = DEFAULT_WALL_FRICTION;

        // Debug
        table->walls[i].is_colliding = false;
    }

    return table;
}

Vector2 wall_get_outside_normal(const wall_t *wall)
{
    Vector2 normal = {wall->direction.y, -wall->direction.x};
    return normal;
}
