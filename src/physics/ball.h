#ifndef PHYSICS_BALL_H
#define PHYSICS_BALL_H

#include "constants.h"
#include "planets.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>

typedef struct ball {
    planet_t planet;

    float radius;

    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;

    float rotation;
    float angular_velocity;
    float angular_acceleration;

    Vector2 sum_forces;
    float sum_torques;

    float mass;
    float inverse_mass;

    float inertia;
    float inverse_inertia;

    float restitution;
    float friction;

    // Debug
    bool is_colliding;
} ball_t;

ball_t *ball_create(const planet_t planet, const Vector2 position);

int ball_is_static(const ball_t *ball);

void ball_add_force(ball_t *ball, const Vector2 force);
void ball_add_torque(ball_t *ball, const float torque);
void ball_clear_forces(ball_t *ball);

void ball_apply_impulse_linear(ball_t *ball, const Vector2 j);
void ball_apply_impulse_angular(ball_t *ball, const float j);
void ball_apply_impulse_at_point(ball_t *ball, const Vector2 j,
                                 const Vector2 r);
void ball_reset_impulses(ball_t *ball);

void ball_integrate_forces(ball_t *ball, const float dt);
void ball_integrate_velocities(ball_t *ball, const float dt);

Vector2 ball_local_space_to_world_space(ball_t *ball,
                                        const Vector2 localPosition);
Vector2 ball_world_space_to_local_space(ball_t *ball,
                                        const Vector2 worldPosition);

#endif
