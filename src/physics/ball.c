#include "ball.h"
#include "constants.h"
#include "raymath.h"
#include <stdlib.h>

ball_t *ball_create(const unsigned int id, const Vector2 position,
                    const float radius, const float mass)
{
    ball_t *ball = malloc(sizeof(ball_t));

    ball->id = id;

    ball->radius = radius;

    ball->position = position;
    ball->velocity = Vector2Zero();
    ball->acceleration = Vector2Zero();

    ball->rotation = 0.f;
    ball->angular_velocity = 0.f;
    ball->angular_acceleration = 0.f;

    ball->mass = mass >= 0 ? mass : 0;
    if (mass > 0)
        ball->inverse_mass = 1.f / ball->mass;
    else
        ball->inverse_mass = 0.f;

    ball->inertia = 0.5 * radius * radius * mass;
    if (ball->inertia > 0)
        ball->inverse_inertia = 1.f / ball->inertia;
    else
        ball->inverse_inertia = 0.f;

    ball->restitution = DEFAULT_BALL_RESTITUTION;
    ball->friction = DEFAULT_BALL_FRICTION;

    // Debug
    ball->is_colliding = false;

    return ball;
}

int ball_is_static(const ball_t *ball)
{
    const float epsilon = 0.005f;
    return fabs(ball->inverse_mass) < epsilon;
}

void ball_add_force(ball_t *ball, const Vector2 force)
{
    ball->sum_forces = Vector2Add(ball->sum_forces, force);
}

void ball_add_torque(ball_t *ball, const float torque)
{
    ball->sum_torques += torque;
}

void ball_clear_forces(ball_t *ball)
{
    ball->sum_forces = Vector2Zero();
    ball->sum_torques = 0;
}

void ball_apply_impulse_linear(ball_t *ball, const Vector2 j)
{
    if (ball_is_static(ball))
        return;

    ball->velocity =
        Vector2Add(ball->velocity, Vector2Scale(j, ball->inverse_mass));
}

void ball_apply_impulse_angular(ball_t *ball, const float j)
{
    if (ball_is_static(ball))
        return;

    ball->angular_velocity += j * ball->inverse_inertia;
}

void ball_apply_impulse_at_point(ball_t *ball, const Vector2 j, const Vector2 r)
{
    if (ball_is_static(ball))
        return;

    ball->velocity =
        Vector2Add(ball->velocity, Vector2Scale(j, ball->inverse_mass));
    ball->angular_velocity += CROSS(r, j) * ball->inverse_inertia;
}

void ball_reset_impulses(ball_t *ball)
{
    ball->velocity = Vector2Zero();
    ball->angular_velocity = 0;
}

void ball_integrate_forces(ball_t *ball, const float dt)
{
    if (ball_is_static(ball))
        return;

    ball->acceleration = Vector2Scale(ball->sum_forces, ball->inverse_mass);
    ball->angular_acceleration = ball->sum_torques * ball->inverse_inertia;

    ball->velocity =
        Vector2Add(ball->velocity, Vector2Scale(ball->acceleration, dt));
    ball->angular_velocity += ball->angular_acceleration * dt;

    ball_clear_forces(ball);
}

void ball_integrate_velocities(ball_t *ball, const float dt)
{
    if (ball_is_static(ball))
        return;

    ball->position =
        Vector2Add(ball->position, Vector2Scale(ball->velocity, dt));
    ball->rotation += ball->angular_velocity * dt;
}

Vector2 ball_local_space_to_world_space(ball_t *ball,
                                        const Vector2 localPosition)
{
    return Vector2Add(ball->position,
                      Vector2Rotate(localPosition, ball->rotation));
}
Vector2 ball_world_space_to_local_space(ball_t *ball,
                                        const Vector2 worldPosition)
{
    return Vector2Rotate(Vector2Subtract(worldPosition, ball->position),
                         -ball->rotation);
}
