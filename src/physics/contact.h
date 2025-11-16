#ifndef PHYSICS_CONTACT_H
#define PHYSICS_CONTACT_H

#include "ball.h"
#include "wall.h"

typedef enum {
  CONTACT_BALL_BALL,
  CONTACT_BALL_WALL,
} ContactType;

typedef struct Contact {
  ContactType type;
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
} Contact;

bool balls_are_colliding(Ball *ball1, Ball *ball2, Contact *contact);
bool ball_wall_are_colliding(Ball *ball, Wall *wall, Contact *contact);

void init_contact_resolution_info(Contact *contact);
void contact_pre_solve(Contact *contact, float dt);
void contact_solve(Contact *contact, float dt);

float get_contact_friction(Contact *contact);
float get_contact_restitution(Contact *contact);
void get_contact_velocities_vector(Contact *contact, float *velocities);
void get_contact_invm_matrix(Contact *contact, float *invM);

#endif
