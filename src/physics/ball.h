#ifndef PHYSICS_BALL_H
#define PHYSICS_BALL_H

#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>

typedef struct Ball {
    float radius;

    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;

    float rotation;
    float angularVelocity;
    float angularAcceleration;

    Vector2 sumForces;
    float sumTorques;

    float mass;
    float inverseMass;

    float inertia;
    float inverseInertia;

    float restitution;
    float friction;

    // Debug
    bool is_colliding;
} Ball;

Ball *ball_create(const Vector2 position, const float radius, const float mass);

int ball_is_static(const Ball *ball);

void ball_add_force(Ball *ball, const Vector2 force);
void ball_add_torque(Ball *ball, const float torque);
void ball_clear_forces(Ball *ball);

void ball_apply_impulse_linear(Ball *ball, const Vector2 j);
void ball_apply_impulse_angular(Ball *ball, const float j);
void ball_apply_impulse_at_point(Ball *ball, const Vector2 j, const Vector2 r);

void ball_integrate_forces(Ball *ball, const float dt);
void ball_integrate_velocities(Ball *ball, const float dt);

Vector2 ball_local_space_to_world_space(Ball *ball,
                                        const Vector2 localPosition);
Vector2 ball_world_space_to_local_space(Ball *ball,
                                        const Vector2 worldPosition);

#endif
