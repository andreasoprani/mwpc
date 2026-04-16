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

    const Vector2 hole_positions[6] = {
        {origin.x, origin.y},                      // TL
        {origin.x + width, origin.y},              // TR
        {origin.x + width, origin.y + height / 2}, // MR
        {origin.x + width, origin.y + height},     // BR
        {origin.x, origin.y + height},             // BL
        {origin.x, origin.y + height / 2},         // ML
    };

    for (int i = 0; i < ARR_LEN(hole_positions); i++) {
        table->holes[i] = (hole_t) {
            .position = hole_positions[i],
            .radius = DEFAULT_HOLE_RADIUS,
        };
    }

    for (int i = 0; i < ARR_LEN(table->walls); i++) {
        const hole_t h0 = table->holes[i];
        const hole_t h1 = table->holes[(i + 1) % 6];

        const Vector2 edge_dir =
            Vector2Normalize(Vector2Subtract(h1.position, h0.position));
        const Vector2 outward = (Vector2) {edge_dir.y, -edge_dir.x};

        table->walls[i] = (wall_t) {
            .vertices[0] = Vector2Add(
                Vector2Add(h0.position, Vector2Scale(outward, h0.radius)),
                Vector2Scale(edge_dir, h0.radius)),
            .vertices[1] = Vector2Add(
                Vector2Add(h1.position, Vector2Scale(outward, h1.radius)),
                Vector2Scale(edge_dir, -h1.radius)),
            .vertices[2] =
                Vector2Add(h1.position, Vector2Scale(edge_dir, -h1.radius)),
            .vertices[3] = Vector2Add(
                Vector2Add(h1.position, Vector2Scale(edge_dir, -2 * h1.radius)),
                Vector2Scale(outward, -h1.radius)),
            .vertices[4] = Vector2Add(
                Vector2Add(h0.position, Vector2Scale(edge_dir, 2 * h0.radius)),
                Vector2Scale(outward, -h0.radius)),
            .vertices[5] =
                Vector2Add(h0.position, Vector2Scale(edge_dir, h0.radius)),
            .restitution = DEFAULT_WALL_RESTITUTION,
            .friction = DEFAULT_WALL_FRICTION,
            .is_colliding = false,
        };
    }

    return table;
}

Vector2 table_get_position(const table_t *table,
                           const Vector2 relative_position)
{
    return (Vector2) {table->origin.x + relative_position.x * table->width,
                      table->origin.y + relative_position.y * table->height};
}

void get_wall_vertices(const int wall_n, const hole_t hole0, const hole_t hole1,
                       Vector2 vertices[4])
{
    switch (wall_n) {
    case 0: // TOP
        vertices[0] = (Vector2) {
            hole0.position.x, hole0.position.y + hole0.radius + WALL_THICKNESS};
        vertices[1] = (Vector2) {
            hole1.position.x, hole1.position.y + hole1.radius + WALL_THICKNESS};
        vertices[2] = (Vector2) {
            hole1.position.x, hole1.position.y - hole1.radius - WALL_THICKNESS};
        vertices[3] = (Vector2) {
            hole0.position.x, hole0.position.y - hole0.radius - WALL_THICKNESS};
        break;
    case 1: // RIGHT (TOP)
        vertices[0] = (Vector2) {
            hole1.position.x + hole1.radius + WALL_THICKNESS, hole1.position.y};
        vertices[1] =
            (Vector2) {hole1.position.x + hole1.radius + WALL_THICKNESS,
                       hole1.position.y + hole1.radius + WALL_THICKNESS};
        vertices[2] =
            (Vector2) {hole1.position.x - hole1.radius - WALL_THICKNESS,
                       hole1.position.y + hole1.radius + WALL_THICKNESS};
        vertices[3] = (Vector2) {
            hole1.position.x - hole1.radius - WALL_THICKNESS, hole1.position.y};
        break;
    case 2: // RIGHT (BOTTOM)
        vertices[0] =
            (Vector2) {hole1.position.x + hole1.radius + WALL_THICKNESS,
                       hole1.position.y - hole1.radius - WALL_THICKNESS};
        vertices[1] =
            (Vector2) {hole1.position.x + hole1.radius + WALL_THICKNESS,
                       hole1.position.y - hole1.radius - WALL_THICKNESS};
        vertices[2] =
            (Vector2) {hole1.position.x - hole1.radius - WALL_THICKNESS,
                       hole1.position.y - hole1.radius - WALL_THICKNESS};
        vertices[3] =
            (Vector2) {hole1.position.x - hole1.radius - WALL_THICKNESS,
                       hole1.position.y - hole1.radius - WALL_THICKNESS};
        break;
    case 3: // BOTTOM
        vertices[0] =
            (Vector2) {hole0.position.x - hole0.radius - WALL_THICKNESS,
                       hole0.position.y - hole0.radius - WALL_THICKNESS};
        vertices[1] =
            (Vector2) {hole1.position.x - hole1.radius - WALL_THICKNESS,
                       hole1.position.y - hole1.radius - WALL_THICKNESS};
        vertices[2] =
            (Vector2) {hole1.position.x - hole1.radius - WALL_THICKNESS,
                       hole1.position.y - hole1.radius - WALL_THICKNESS};
        vertices[3] =
            (Vector2) {hole0.position.x - hole0.radius - WALL_THICKNESS,
                       hole0.position.y - hole0.radius - WALL_THICKNESS};
        break;
    case 4: // LEFT (BOTTOM)
        vertices[0] =
            (Vector2) {hole0.position.x - hole0.radius - WALL_THICKNESS,
                       hole0.position.y - hole0.radius - WALL_THICKNESS};
        vertices[1] =
            (Vector2) {hole0.position.x - hole0.radius - WALL_THICKNESS,
                       hole0.position.y - hole0.radius - WALL_THICKNESS};
        vertices[2] =
            (Vector2) {hole0.position.x - hole0.radius - WALL_THICKNESS,
                       hole0.position.y - hole0.radius - WALL_THICKNESS};
        vertices[3] =
            (Vector2) {hole0.position.x - hole0.radius - WALL_THICKNESS,
                       hole0.position.y - hole0.radius - WALL_THICKNESS};
        break;
    case 5: // LEFT (TOP)
        vertices[0] =
            (Vector2) {hole0.position.x - hole0.radius - WALL_THICKNESS,
                       hole0.position.y - hole0.radius - WALL_THICKNESS};
        vertices[1] =
            (Vector2) {hole0.position.x - hole0.radius - WALL_THICKNESS,
                       hole0.position.y - hole0.radius - WALL_THICKNESS};
        vertices[2] =
            (Vector2) {hole0.position.x - hole0.radius - WALL_THICKNESS,
                       hole0.position.y - hole0.radius - WALL_THICKNESS};
        vertices[3] =
            (Vector2) {hole0.position.x - hole0.radius - WALL_THICKNESS,
                       hole0.position.y - hole0.radius - WALL_THICKNESS};
        break;
    }
}
