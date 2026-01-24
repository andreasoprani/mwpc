#ifndef PHYSICS_CONTACT_H
#define PHYSICS_CONTACT_H

#include "ball.h"
#include "wall.h"

typedef enum {
    CONTACT_BALL_BALL,
    CONTACT_BALL_WALL,
} contact_type_t;

typedef struct contact {
    contact_type_t type;
    ball_t *ball;
    union {
        ball_t *ball;
        wall_t *wall;
    } other;

    Vector2 start;
    Vector2 end;
    Vector2 normal;
    float depth;

    float jacobian[2 * 6]; // 2x6 matrix
    float cached_lambda[2];
    float bias;
} contact_t;

bool balls_are_colliding(ball_t *ball1, ball_t *ball2, contact_t *contact);
bool ball_wall_are_colliding(ball_t *ball, wall_t *wall, contact_t *contact);

void init_contact_resolution_info(contact_t *contact);
void contact_pre_solve(contact_t *contact, float dt);
void contact_solve(contact_t *contact, float dt);

float get_contact_friction(contact_t *contact);
float get_contact_restitution(contact_t *contact);
void get_contact_velocities_vector(contact_t *contact, float *velocities);
void get_contact_invm_matrix(contact_t *contact, float *invM);

#endif
