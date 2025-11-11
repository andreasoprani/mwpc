#ifndef PHYSICS_COLLISION_H
#define PHYSICS_COLLISION_H

#include "ball.h"
#include "wall.h"

typedef enum {
  COLLISION_BALL_BALL,
  COLLISION_BALL_WALL,
} CollisionType;

typedef struct Contact {
  CollisionType type;
  Ball *ball;
  union {
    Ball *ball;
    Wall *wall;
  } other;

  Vector2 start;
  Vector2 end;
  Vector2 normal;
  float depth;

  float jacobian[2 * 6]; // 2x6 matrix
  float cachedLambda[2];
  float bias;
  float friction;
} Contact;

bool balls_are_colliding(Ball *ball1, Ball *ball2, Contact *contact);
bool ball_wall_are_colliding(Ball *ball, Wall *wall, Contact *contact);

void contact_pre_solve(Contact *contact, float dt);
void contact_solve(Contact *contact, float dt);

#endif
