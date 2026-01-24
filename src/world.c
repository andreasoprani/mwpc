#include "world.h"
#include "physics/ball.h"
#include "physics/constants.h"
#include "physics/contact.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

World *world_create(Table *table)
{
    World *world = malloc(sizeof(World));
    int balls_capacity = 10;
    world->balls = malloc(balls_capacity * sizeof(Ball));
    world->balls_capacity = balls_capacity;
    world->ballsLength = 0;

    int contacts_capacity = balls_capacity * (balls_capacity - 1) +
                            balls_capacity * table->num_walls;
    world->contacts = malloc(contacts_capacity * sizeof(Contact));
    world->contacts_capacity = contacts_capacity;
    world->contacts_length = 0;

    world->gravityEnabled = false;

    world->table = table;
    return world;
};

void world_destroy(World *world)
{
    free(world->balls);
    free(world);
}

void world_update(World *world, float dt)
{
    world->contacts_length = 0;

    // Add all gravitational forces
    if (world->gravityEnabled) {
        for (int i = 0; i < world->ballsLength - 1; i++) {
            Ball *ball_i = &world->balls[i];
            for (int j = i + 1; j < world->ballsLength; j++) {
                Ball *ball_j = &world->balls[j];
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

    for (int i = 0; i < world->ballsLength; i++)
        ball_integrate_forces(&world->balls[i], dt);

    for (int i = 0; i < world->ballsLength; i++)
        (&world->balls[i])->is_colliding = false;

    for (int i = 0; i < world->table->num_walls; i++)
        (&world->table->walls[i])->is_colliding = false;

    for (int i = 0; i < world->ballsLength - 1; i++) {
        Ball *ball_i = &world->balls[i];
        for (int j = i + 1; j < world->ballsLength; j++) {
            Ball *ball_j = &world->balls[j];

            Contact *contact = &world->contacts[world->contacts_length];
            if (balls_are_colliding(ball_i, ball_j, contact)) {
                world->contacts_length++;

                ball_i->is_colliding = true;
                ball_j->is_colliding = true;
            }
        }
    }

    for (int i = 0; i < world->ballsLength; i++) {
        Ball *ball = &world->balls[i];
        for (int j = 0; j < world->table->num_walls; j++) {
            Wall *wall = &world->table->walls[j];

            Contact *contact = &world->contacts[world->contacts_length];
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

    for (int i = 0; i < world->ballsLength; i++)
        ball_integrate_velocities(&world->balls[i], dt);
}

void world_add_ball(World *world, Ball *ball)
{
    if (world->ballsLength == world->balls_capacity) {
        world->balls_capacity *= 2;
        world->balls = (Ball *) realloc(world->balls,
                                        world->balls_capacity * sizeof(Ball));

        world->contacts_capacity =
            world->balls_capacity * (world->balls_capacity - 1) +
            world->balls_capacity * world->table->num_walls;
        world->contacts = (Contact *) realloc(
            world->contacts, world->contacts_capacity * sizeof(Contact));
    }
    world->balls[world->ballsLength++] = *ball;
}

void world_toggle_gravity(World *world)
{
    world->gravityEnabled = !world->gravityEnabled;
    printf("Gravity: %s\n", world->gravityEnabled ? "ON" : "OFF");
}
