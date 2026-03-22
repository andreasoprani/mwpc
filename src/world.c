#include "world.h"
#include "constants.h"
#include "physics/ball.h"
#include "physics/contact.h"
#include "physics/planets.h"
#include "physics/wall.h"
#include "raylib.h"
#include "raymath.h"
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

    for (unsigned int i = 1; i < num_other_planets; i++) {
        planet_t planet = other_planets[i];
        ball_t *last_ball = world->balls[i];
        Vector2 position = {last_ball->position.x,
                            last_ball->position.y + last_ball->radius +
                                get_planet_radius(planet)};
        ball_t *ball = ball_create(planet, position);
        world_add_ball(world, ball);
    }
}

void world_toggle_gravity(world_t *world)
{
    world->gravity_enabled = !world->gravity_enabled;
    printf("Gravity: %s\n", world->gravity_enabled ? "ON" : "OFF");
}
