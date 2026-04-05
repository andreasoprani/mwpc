#include "table.h"
#include "constants.h"
#include "raymath.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

Vector2 wall_get_outside_normal(const wall_t *wall)
{
    Vector2 normal = {wall->direction.y, -wall->direction.x};
    return normal;
}

table_t *table_create(const int num_walls, const Vector2 *vertices)
{
    table_t *table = malloc(sizeof(table_t));
    table->num_walls = num_walls;
    table->walls = malloc(num_walls * sizeof(wall_t));
    for (int i = 0; i < num_walls; i++) {
        table->walls[i].start = vertices[i];
        table->walls[i].direction = Vector2Normalize(
            Vector2Subtract(vertices[(i + 1) % num_walls], vertices[i]));
        table->walls[i].length =
            Vector2Distance(vertices[i], vertices[(i + 1) % num_walls]);

        table->walls[i].restitution = DEFAULT_WALL_RESTITUTION;
        table->walls[i].friction = DEFAULT_WALL_FRICTION;

        // Debug
        table->walls[i].is_colliding = false;
    }

    return table;
}

Vector2 table_get_position(const table_t *table,
                           const Vector2 relative_position)
{
    float min_x = FLT_MAX;
    float max_x = 0.0f;
    float min_y = FLT_MAX;
    float max_y = 0.0f;

    for (int i = 0; i < table->num_walls; i++) {
        min_x = fminf(min_x, table->walls[i].start.x);
        max_x = fmaxf(max_x, table->walls[i].start.x);
        min_y = fminf(min_y, table->walls[i].start.y);
        max_y = fmaxf(max_y, table->walls[i].start.y);
    }

    return (Vector2) {min_x + relative_position.x * (max_x - min_x),
                      min_y + relative_position.y * (max_y - min_y)};
}
