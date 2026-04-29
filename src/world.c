#include "world.h"
#include "constants.h"
#include "physics/ball.h"
#include "physics/constants.h"
#include "physics/contact.h"
#include "physics/planets.h"
#include "physics/table.h"
#include "raylib.h"
#include "raymath.h"
#include "textures.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define WORLD_TABLE_HEIGHT (720.0f - 2.0f * TABLE_H_PAD)
#define WORLD_TABLE_WIDTH (WORLD_TABLE_HEIGHT / TABLE_RATIO)

world_t *world_create()
{
    world_t *world = (world_t *) malloc(sizeof(world_t));

    world->balls_count = 0;
    world->contacts_count = 0;

    world->gravity_enabled = false;

    table_setup(&world->table, Vector2Zero(), WORLD_TABLE_WIDTH,
                WORLD_TABLE_HEIGHT);

    world_place_all_balls(world);

    return world;
};

void world_update(world_t *world, const float dt)
{
    world->contacts_count = 0;

    // Add all gravitational forces
    if (world->gravity_enabled) {
        for (int i = 0; i < world->balls_count - 1; i++) {
            ball_t *ball_i = &world->balls[i];
            for (int j = i + 1; j < world->balls_count; j++) {
                ball_t *ball_j = &world->balls[j];
                const Vector2 distance =
                    Vector2Subtract(ball_j->position, ball_i->position);
                const float distance_squared = Vector2LengthSqr(distance);

                // Calculate the force between the two balls
                float force_magnitude = GRAVITATIONAL_CONSTANT *
                                        world->balls[i].mass *
                                        world->balls[j].mass / distance_squared;

                Vector2 force =
                    Vector2Scale(Vector2Normalize(distance), force_magnitude);

                ball_add_force(ball_i, force);
                ball_add_force(ball_j, Vector2Scale(force, -1));
            }
        }
    }

    for (int i = 0; i < world->balls_count; i++)
        ball_integrate_forces(&world->balls[i], dt);

    for (int i = 0; i < world->balls_count; i++)
        (&world->balls[i])->is_colliding = false;

    for (int i = 0; i < ARR_LEN(world->table.walls); i++)
        (&world->table.walls[i])->is_colliding = false;

    for (int i = 0; i < world->balls_count - 1; i++) {
        ball_t *ball_i = &world->balls[i];
        for (int j = i + 1; j < world->balls_count; j++) {
            ball_t *ball_j = &world->balls[j];

            contact_t *contact = &world->contacts[world->contacts_count];
            if (balls_are_colliding(ball_i, ball_j, contact)) {
                world->contacts_count++;

                ball_i->is_colliding = true;
                ball_j->is_colliding = true;
            }
        }
    }

    for (int i = 0; i < world->balls_count; i++) {
        ball_t *ball = (&world->balls[i]);
        for (int j = 0; j < ARR_LEN(world->table.walls); j++) {
            wall_t *wall = &world->table.walls[j];

            contact_t *contact = &world->contacts[world->contacts_count];
            if (ball_wall_are_colliding(ball, wall, contact)) {
                world->contacts_count++;

                ball->is_colliding = true;
                wall->is_colliding = true;
            }
        }
    }

    for (int i = 0; i < world->contacts_count; i++) {
        contact_pre_solve(&world->contacts[i], dt);
    }

    for (int n = 0; n < CONTACT_SOLVE_ITERATIONS; n++) {
        for (int i = 0; i < world->contacts_count; i++) {
            contact_solve(&world->contacts[i], dt);
        }
    }

    for (int i = 0; i < world->contacts_count; i++) {
        contact_correct_position(&world->contacts[i], dt);
    }

    for (int i = 0; i < world->balls_count; i++)
        ball_integrate_velocities(&world->balls[i], dt);

    for (int i = 0; i < ARR_LEN(world->table.holes); i++) {
        (&world->table.holes[i])->rotation += HOLE_ROTATION_SPEED;
    }

    { // Check for balls inside holes and remove them
        unsigned int to_remove = 0;
        for (int b = 0; b < world->balls_count; b++) {
            ball_t *ball = &world->balls[b];
            for (int h = 0; h < ARR_LEN(world->table.holes); h++) {
                hole_t *hole = &world->table.holes[h];
                if (Vector2Distance(ball->position, hole->position) <
                    hole->radius) {
                    to_remove |= PLANET_BIT(ball->planet);
                }
            }
        }

        for (int p = 0; p < NUM_PLANETS; p++) {
            if (to_remove & PLANET_BIT(p)) {
                int ball_index = 0;
                for (int i = 0; i < world->balls_count; i++) {
                    if (world->balls[i].planet == p) {
                        ball_index = i;
                        break;
                    }
                }

                if (ball_index < world->balls_count - 1) {
                    world->balls[ball_index] =
                        world->balls[world->balls_count - 1];
                }
                world->balls_count -= 1;
            }
        }
    }

    if (world->balls_count == 0) {
    }
}

bool is_position_in_cone(const Vector2 position, const Vector2 apex)
{
    Vector2 direction = Vector2Subtract(position, apex);
    float angle = atan2f(fabsf(direction.x), -direction.y);
    return angle <= DEG2RAD * CONE_ANGLE / 2 + 0.001f;
}

bool is_position_valid(const world_t *world, const Vector2 position,
                       const float radius)
{
    for (int i = 0; i < world->balls_count; i++) {
        const ball_t *ball = &world->balls[i];
        float distance = Vector2DistanceSqr(position, ball->position);
        float min_distance = ball->radius + radius;
        if (distance < min_distance * min_distance) {
            return false;
        }
    }

    // TODO check walls, could be useful to use the same logic as contacts (also
    // for balls) but it requires initializing the ball to create contact

    return is_position_in_cone(position,
                               table_get_position(world->table, BALLS_APEX));
}

void world_place_all_balls(world_t *world)
{
    const float balls_gap = BALLS_GAP;

    world->balls[world->balls_count++] = ball_create(
        EARTH, table_get_position(world->table, CONTROLLED_BALL_POSITION));

    const Vector2 apex_position = table_get_position(world->table, BALLS_APEX);

    static const planet_t other_planets[] = {MERCURY, VENUS,  MARS,    JUPITER,
                                             SATURN,  URANUS, NEPTUNE, PLUTO};

    world->balls[world->balls_count++] =
        ball_create(other_planets[0], apex_position);

    const Vector2 left_edge_dir =
        Vector2Rotate((Vector2) {0, -1}, DEG2RAD * CONE_ANGLE / 2);
    const Vector2 right_edge_dir =
        Vector2Rotate((Vector2) {0, -1}, -DEG2RAD * CONE_ANGLE / 2);

    for (unsigned int i = 1; i < ARR_LEN(other_planets); i++) {
        planet_t planet = other_planets[i];
        float planet_radius = get_planet_radius(planet);

        Vector2 best_position =
            table_get_position(world->table, (Vector2) {0.5f, 0.0f});
        float best_distance = Vector2DistanceSqr(best_position, apex_position);

        // Positions between balls
        for (int j = 1; j < world->balls_count; j++) {
            for (int k = j + 1; k < world->balls_count; k++) {
                const ball_t *ball1 = &world->balls[j];
                const ball_t *ball2 = &world->balls[k];
                const float rd1 = ball1->radius + planet_radius + balls_gap;
                const float rd2 = ball2->radius + planet_radius + balls_gap;
                const float d =
                    Vector2Distance(ball1->position, ball2->position);
                if (d > (rd1 + rd2) || d < fabsf(rd1 - rd2)) {
                    continue;
                }
                const float a = (rd1 * rd1 - rd2 * rd2 + d * d) / (2.0f * d);
                const float h = sqrtf(rd1 * rd1 - a * a);
                const Vector2 dir = Vector2Scale(
                    Vector2Subtract(ball2->position, ball1->position),
                    1.0f / d);
                const Vector2 mid =
                    Vector2Add(ball1->position, Vector2Scale(dir, a));
                const Vector2 perp = {-dir.y, dir.x};
                const Vector2 candidates[2] = {
                    Vector2Add(mid, Vector2Scale(perp, h)),
                    Vector2Subtract(mid, Vector2Scale(perp, h)),
                };
                for (int c = 0; c < 2; c++) {
                    if (!is_position_valid(world, candidates[c], planet_radius))
                        continue;
                    float dist =
                        Vector2DistanceSqr(candidates[c], apex_position);
                    if (dist < best_distance) {
                        best_distance = dist;
                        best_position = candidates[c];
                    }
                }
            }
        }

        // Positions on the cone edges
        for (int j = 1; j < world->balls_count; j++) {
            const ball_t *ball = &world->balls[j];
            const float required_distance =
                ball->radius + planet_radius + balls_gap;

            for (int ed = 0; ed <= 1; ed++) {
                const Vector2 edge_dir =
                    (ed == 0 ? left_edge_dir : right_edge_dir);
                const Vector2 d =
                    Vector2Subtract(apex_position, ball->position);
                float b = 2.0f * Vector2DotProduct(d, edge_dir);
                float c = Vector2DotProduct(d, d) -
                          required_distance * required_distance;
                float discriminant = b * b - 4.0f * c;
                if (discriminant < 0.0f) {
                    continue;
                }
                float sqrt_discriminant = sqrtf(discriminant);
                const float ts[2] = {
                    (-b + sqrt_discriminant) / 2.0f,
                    (-b - sqrt_discriminant) / 2.0f,
                };
                for (int k = 0; k < 2; k++) {
                    const Vector2 closest = Vector2Add(
                        apex_position, Vector2Scale(edge_dir, ts[k]));
                    if (!is_position_valid(world, closest, planet_radius)) {
                        continue;
                    }
                    const float closest_distance =
                        Vector2DistanceSqr(closest, apex_position);

                    if (closest_distance < best_distance) {
                        best_distance = closest_distance;
                        best_position = closest;
                    }
                }
            }
        }

        world->balls[world->balls_count++] = ball_create(planet, best_position);
    }
}
