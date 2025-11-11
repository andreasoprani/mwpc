#include "world.h"
#include "physics/ball.h"
#include "physics/collision.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

const float GRAVITATIONAL_CONSTANT = 20;
const float CONTACT_SOLVE_ITERATIONS = 10;

World *world_create(Table *table) {
  World *world = (World *)malloc(sizeof(World));
  world->balls = (Ball *)malloc(100 * sizeof(Ball));
  world->ballsCapacity = 10;
  world->ballsLength = 0;

  world->table = table;
  return world;
};

void world_destroy(World *world) {
  free(world->balls);
  free(world);
}

void world_update(World *world, float dt) {
  // Add all gravitational forces
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

      Vector2 force = Vector2Scale(Vector2Normalize(distance), forceMagnitude);

      ball_add_force(ball_i, force);
      ball_add_force(ball_j, Vector2Scale(force, -1));
    }
  }

  for (int i = 0; i < world->ballsLength; i++)
    ball_integrate_forces(&world->balls[i], dt);

  Contact contacts[world->ballsLength * (world->ballsLength - 1) +
                   (world->ballsLength * world->table->num_walls)];
  int num_contacts = 0;

  for (int i = 0; i < world->ballsLength; i++)
    (&world->balls[i])->isColliding = false;

  for (int i = 0; i < world->table->num_walls; i++)
    (&world->table->walls[i])->isColliding = false;

  for (int i = 0; i < world->ballsLength - 1; i++) {
    Ball *ball_i = &world->balls[i];
    for (int j = i + 1; j < world->ballsLength; j++) {
      Ball *ball_j = &world->balls[j];

      Contact *contact = &contacts[num_contacts];
      if (balls_are_colliding(ball_i, ball_j, contact)) {
        num_contacts++;

        ball_i->isColliding = true;
        ball_j->isColliding = true;
      }
    }
  }

  for (int i = 0; i < world->ballsLength; i++) {
    Ball *ball = &world->balls[i];
    for (int j = 0; j < world->table->num_walls; j++) {
      Wall *wall = &world->table->walls[j];

      Contact *contact = &contacts[num_contacts];
      if (ball_wall_are_colliding(ball, wall, contact)) {
        // num_contacts++;

        ball->isColliding = true;
        wall->isColliding = true;
      }
    }
  }

  for (int i = 0; i < num_contacts; i++) {
    contact_pre_solve(&contacts[i], dt);
  }

  for (int n = 0; n < CONTACT_SOLVE_ITERATIONS; n++) {
    for (int i = 0; i < num_contacts; i++) {
      contact_solve(&contacts[i], dt);
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
  }
  world->balls[world->ballsLength++] = *ball;
}
