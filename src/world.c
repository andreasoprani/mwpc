#include "world.h"
#include "constants.h"
#include "physics/ball.h"
#include "physics/contact.h"
#include "physics/planets.h"
#include "physics/table.h"
#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

world_t *world_create(table_t *table)
{
    world_t *world = malloc(sizeof(world_t));
    int balls_capacity = 10;
    world->balls = malloc(balls_capacity * sizeof(ball_t *));
    world->balls_capacity = balls_capacity;
    world->balls_length = 0;

    int contacts_capacity = balls_capacity * (balls_capacity - 1) +
                            balls_capacity * table->num_walls;
    world->contacts = malloc(contacts_capacity * sizeof(contact_t));
    world->contacts_capacity = contacts_capacity;
    world->contacts_length = 0;

    world->gravity_enabled = false;

    world->table = table;
    return world;
};

void world_destroy(world_t *world)
{
    for (unsigned int i = 0; i < world->balls_length; i++)
        free(world->balls[i]);
    free(world->balls);
    free(world->contacts);
    for (int i = 0; i < world->table->num_walls; i++)
        free(world->table->walls[i].vertices);
    free(world->table->walls);
    free(world->table);
    free(world);
}

void world_update(world_t *world, const float dt)
{
    world->contacts_length = 0;

    // Add all gravitational forces
    if (world->gravity_enabled) {
        for (int i = 0; i < world->balls_length - 1; i++) {
            ball_t *ball_i = world->balls[i];
            for (int j = i + 1; j < world->balls_length; j++) {
                ball_t *ball_j = world->balls[j];
                const Vector2 distance =
                    Vector2Subtract(ball_j->position, ball_i->position);
                const float distance_squared = Vector2LengthSqr(distance);

                // Calculate the force between the two balls
                float force_magnitude =
                    GRAVITATIONAL_CONSTANT * world->balls[i]->mass *
                    world->balls[j]->mass / distance_squared;

                Vector2 force =
                    Vector2Scale(Vector2Normalize(distance), force_magnitude);

                ball_add_force(ball_i, force);
                ball_add_force(ball_j, Vector2Scale(force, -1));
            }
        }
    }

    for (int i = 0; i < world->balls_length; i++)
        ball_integrate_forces(world->balls[i], dt);

    for (int i = 0; i < world->balls_length; i++)
        world->balls[i]->is_colliding = false;

    for (int i = 0; i < world->table->num_walls; i++)
        (&world->table->walls[i])->is_colliding = false;

    for (int i = 0; i < world->balls_length - 1; i++) {
        ball_t *ball_i = world->balls[i];
        for (int j = i + 1; j < world->balls_length; j++) {
            ball_t *ball_j = world->balls[j];

            contact_t *contact = &world->contacts[world->contacts_length];
            if (balls_are_colliding(ball_i, ball_j, contact)) {
                world->contacts_length++;

                ball_i->is_colliding = true;
                ball_j->is_colliding = true;
            }
        }
    }

    for (int i = 0; i < world->balls_length; i++) {
        ball_t *ball = world->balls[i];
        for (int j = 0; j < world->table->num_walls; j++) {
            wall_t *wall = &world->table->walls[j];

            contact_t *contact = &world->contacts[world->contacts_length];
            if (ball_wall_are_colliding(ball, wall, contact)) {
                world->contacts_length++;

                ball->is_colliding = true;
                wall->is_colliding = true;
            }
        }
    }

    for (int i = 0; i < world->contacts_length; i++) {
        contact_pre_solve(&world->contacts[i], dt);
    }

    for (int n = 0; n < CONTACT_SOLVE_ITERATIONS; n++) {
        for (int i = 0; i < world->contacts_length; i++) {
            contact_solve(&world->contacts[i], dt);
        }
    }

    for (int i = 0; i < world->contacts_length; i++) {
        contact_correct_position(&world->contacts[i], dt);
    }

    for (int i = 0; i < world->balls_length; i++)
        ball_integrate_velocities(world->balls[i], dt);
}

void world_toggle_gravity(world_t *world)
{
    world->gravity_enabled = !world->gravity_enabled;
    printf("Gravity: %s\n", world->gravity_enabled ? "ON" : "OFF");
}

void world_add_ball(world_t *world, ball_t *ball)
{
    if (world->balls_length == world->balls_capacity) {
        world->balls_capacity *= 2;
        world->balls = (ball_t **) realloc(world->balls, world->balls_capacity *
                                                             sizeof(ball_t *));

        world->contacts_capacity =
            world->balls_capacity * (world->balls_capacity - 1) +
            world->balls_capacity * world->table->num_walls;
        world->contacts = (contact_t *) realloc(
            world->contacts, world->contacts_capacity * sizeof(contact_t));
    }
    world->balls[world->balls_length++] = ball;
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
    for (int i = 0; i < world->balls_length; i++) {
        const ball_t *ball = world->balls[i];
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

void world_place_all_balls(world_t *world, const planet_t controlled_planet,
                           const planet_t *other_planets,
                           const unsigned int num_other_planets)
{
    ball_t *controlled_ball =
        ball_create(controlled_planet,
                    table_get_position(world->table, CONTROLLED_BALL_POSITION));
    world_add_ball(world, controlled_ball);

    if (!num_other_planets)
        return;

    const Vector2 apex_position = table_get_position(world->table, BALLS_APEX);
    ball_t *apex_ball = ball_create(other_planets[0], apex_position);
    world_add_ball(world, apex_ball);

    const Vector2 left_edge_dir =
        Vector2Rotate((Vector2) {0, -1}, DEG2RAD * CONE_ANGLE / 2);
    const Vector2 right_edge_dir =
        Vector2Rotate((Vector2) {0, -1}, -DEG2RAD * CONE_ANGLE / 2);

    for (unsigned int i = 1; i < num_other_planets; i++) {
        planet_t planet = other_planets[i];
        float planet_radius = get_planet_radius(planet);

        Vector2 best_position =
            table_get_position(world->table, (Vector2) {0.5f, 0.0f});
        float best_distance = Vector2DistanceSqr(best_position, apex_position);

        // Positions between balls
        for (int j = 1; j < world->balls_length; j++) {
            for (int k = j + 1; k < world->balls_length; k++) {
                const ball_t *ball1 = world->balls[j];
                const ball_t *ball2 = world->balls[k];
                const float rd1 = ball1->radius + planet_radius + BALLS_GAP;
                const float rd2 = ball2->radius + planet_radius + BALLS_GAP;
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
        for (int j = 1; j < world->balls_length; j++) {
            const ball_t *ball = world->balls[j];
            const float required_distance =
                ball->radius + planet_radius + BALLS_GAP;

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

        ball_t *ball = ball_create(planet, best_position);
        world_add_ball(world, ball);
    }
}
