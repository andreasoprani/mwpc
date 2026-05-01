#include "table.h"
#include "constants.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>

void table_setup(table_t *table, float width, float height)
{
    table->width = width;
    table->height = height;

    // Table boundary anchor points (corners and mid-edges)
    const Vector2 anchors[6] = {
        {0, 0},             // TL
        {width, 0},         // TR
        {width, height / 2}, // MR
        {width, height},     // BR
        {0, height},         // BL
        {0, height / 2},     // ML
    };

    // Per-anchor outward normal = average of the two adjacent wall normals
    Vector2 anchor_outward[6] = {0};
    for (int i = 0; i < 6; i++) {
        const Vector2 dir =
            Vector2Normalize(Vector2Subtract(anchors[(i + 1) % 6], anchors[i]));
        const Vector2 wall_n = {dir.y, -dir.x};
        anchor_outward[i] = Vector2Add(anchor_outward[i], wall_n);
        anchor_outward[(i + 1) % 6] =
            Vector2Add(anchor_outward[(i + 1) % 6], wall_n);
    }
    for (int i = 0; i < 6; i++)
        anchor_outward[i] = Vector2Normalize(anchor_outward[i]);

    // Holes pushed outward so their centers sit outside the table edge.
    // Side holes get more offset since their rail geometry exposes a
    // half-circle inside the table vs only a quarter-circle at corners.
    for (int i = 0; i < 6; i++) {
        const bool is_side = (i == 2 || i == 5);
        const float offset = is_side ? HOLE_OFFSET_SIDE : HOLE_OFFSET_CORNER;
        table->holes[i] = (hole_t) {
            .position =
                Vector2Add(anchors[i], Vector2Scale(anchor_outward[i], offset)),
            .radius = HOLE_RADIUS,
            .rotation = 0,
        };
    }

    // JAW_FLARE_ANGLE must satisfy tan(angle) < HOLE_MOUTH_SIDE /
    // WALL_THICKNESS so that adjacent collinear walls at side holes don't
    // overlap.
    const float flare = tanf(JAW_FLARE_ANGLE * DEG2RAD);
    for (int i = 0; i < ARR_LEN(table->walls); i++) {
        const int i1 = (i + 1) % 6;
        const bool h0_side = (i == 2 || i == 5);
        const bool h1_side = (i1 == 2 || i1 == 5);

        const Vector2 dir =
            Vector2Normalize(Vector2Subtract(anchors[i1], anchors[i]));
        const Vector2 normal = {dir.y, -dir.x};

        const float mouth0 = h0_side ? HOLE_MOUTH_SIDE : HOLE_MOUTH_CORNER;
        const float mouth1 = h1_side ? HOLE_MOUTH_SIDE : HOLE_MOUTH_CORNER;
        const float flare0 = flare;
        const float flare1 = flare;

        // Inner face lies on the anchor edge line, cut back from each anchor
        const Vector2 v_inner0 =
            Vector2Add(anchors[i], Vector2Scale(dir, mouth0));
        const Vector2 v_inner1 =
            Vector2Add(anchors[i1], Vector2Scale(dir, -mouth1));

        // Outer face is wider: flared away from each end's anchor
        const Vector2 v_outer0 = Vector2Add(
            Vector2Add(v_inner0, Vector2Scale(normal, WALL_THICKNESS)),
            Vector2Scale(dir, -WALL_THICKNESS * flare0));
        const Vector2 v_outer1 = Vector2Add(
            Vector2Add(v_inner1, Vector2Scale(normal, WALL_THICKNESS)),
            Vector2Scale(dir, WALL_THICKNESS * flare1));

        // Clockwise: outer0 → outer1 → inner1 → inner0
        table->walls[i] = (wall_t) {
            .vertices[0] = v_outer0,
            .vertices[1] = v_outer1,
            .vertices[2] = v_inner1,
            .vertices[3] = v_inner0,
            .restitution = WALL_RESTITUTION,
            .friction = WALL_FRICTION,
            .is_colliding = false,
        };
    }
}

Vector2 table_get_position(const table_t table, const Vector2 relative_position)
{
    return (Vector2) {relative_position.x * table.width,
                      relative_position.y * table.height};
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
