#ifndef PHYSICS_COLLISION_H
#define PHYSICS_COLLISION_H

#include "ball.h"

typedef struct BallsContact {
  Ball *ball1;
  Ball *ball2;

  Vector2 start;
  Vector2 end;
  Vector2 normal;
  float depth;

  float jacobian[2 * 6]; // 2x6 matrix
  float cachedLambda[2];
  float bias;
  float friction;
} BallsContact;

bool balls_are_colliding(Ball *ball1, Ball *ball2, BallsContact *contact);
void balls_pre_solve_contact(BallsContact *contact, float dt);
void balls_solve_contact(BallsContact *contact, float dt);

#endif
