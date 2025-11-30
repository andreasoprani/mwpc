#include "world.h"
#include "physics/ball.h"
#include "physics/constants.h"
#include "physics/contact.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

World *world_create(Table *table) {
  World *world = malloc(sizeof(World));
  int ballsCapacity = 10;
  world->balls = malloc(ballsCapacity * sizeof(Ball));
  world->ballsCapacity = ballsCapacity;
  world->ballsLength = 0;

  int contactsCapacity =
      ballsCapacity * (ballsCapacity - 1) + ballsCapacity * table->num_walls;
  world->contacts = malloc(contactsCapacity * sizeof(Contact));
  world->contactsCapacity = contactsCapacity;
  world->contactsLength = 0;

  world->gravityEnabled = false;

  world->table = table;
  return world;
};

void world_destroy(World *world) {
  free(world->balls);
  free(world);
}

void world_update(World *world, float dt) {
  world->contactsLength = 0;

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
        float forceMagnitude = GRAVITATIONAL_CONSTANT * world->balls[i].mass *
                               world->balls[j].mass / distance_squared;

        Vector2 force =
            Vector2Scale(Vector2Normalize(distance), forceMagnitude);

        ball_add_force(ball_i, force);
        ball_add_force(ball_j, Vector2Scale(force, -1));
      }
    }
  }

  for (int i = 0; i < world->ballsLength; i++)
    ball_integrate_forces(&world->balls[i], dt);

  for (int i = 0; i < world->ballsLength; i++)
    (&world->balls[i])->isColliding = false;

  for (int i = 0; i < world->table->num_walls; i++)
    (&world->table->walls[i])->isColliding = false;

  for (int i = 0; i < world->ballsLength - 1; i++) {
    Ball *ball_i = &world->balls[i];
    for (int j = i + 1; j < world->ballsLength; j++) {
      Ball *ball_j = &world->balls[j];

      Contact *contact = &world->contacts[world->contactsLength];
      if (balls_are_colliding(ball_i, ball_j, contact)) {
        world->contactsLength++;

        ball_i->isColliding = true;
        ball_j->isColliding = true;
      }
    }
  }

  for (int i = 0; i < world->ballsLength; i++) {
    Ball *ball = &world->balls[i];
    for (int j = 0; j < world->table->num_walls; j++) {
      Wall *wall = &world->table->walls[j];

      Contact *contact = &world->contacts[world->contactsLength];
      if (ball_wall_are_colliding(ball, wall, contact)) {
        world->contactsLength++;

        ball->isColliding = true;
        wall->isColliding = true;
      }
    }
  }

  for (int i = 0; i < world->contactsLength; i++) {
    contact_pre_solve(&world->contacts[i], dt);
  }

  for (int n = 0; n < CONTACT_SOLVE_ITERATIONS; n++) {
    for (int i = 0; i < world->contactsLength; i++) {
      contact_solve(&world->contacts[i], dt);
    }
  }

  for (int i = 0; i < world->ballsLength; i++)
    ball_integrate_velocities(&world->balls[i], dt);
}

void world_add_ball(World *world, Ball *ball) {
  if (world->ballsLength == world->ballsCapacity) {
    world->ballsCapacity *= 2;
    world->balls =
        (Ball *)realloc(world->balls, world->ballsCapacity * sizeof(Ball));

    world->contactsCapacity =
        world->ballsCapacity * (world->ballsCapacity - 1) +
        world->ballsCapacity * world->table->num_walls;
    world->contacts = (Contact *)realloc(
        world->contacts, world->contactsCapacity * sizeof(Contact));
  }
  world->balls[world->ballsLength++] = *ball;
}

void worldToggleGravity(World *world) {
  world->gravityEnabled = !world->gravityEnabled;
  printf("Gravity: %s\n", world->gravityEnabled ? "ON" : "OFF");
}
