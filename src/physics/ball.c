#include "ball.h"
#include "constants.h"
#include "planets.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

ball_t ball_create(const planet_t planet, const Vector2 position)
{
    const float mass = get_planet_mass(planet);
    const float radius = get_planet_radius(planet);
    const float inertia = 0.5 * radius * radius * mass;

    printf("Planet: %s, radius: %f, mass: %f\n", get_planet_name(planet),
           radius, mass);
    return (ball_t) {
        .planet = planet,

        .radius = radius,
        .mass = mass,
        .inverse_mass = (mass > 0) ? 1.0f / mass : 0.0f,

        .inertia = 0.5 * radius * radius * mass,
        .inverse_inertia = (inertia > 0) ? 1.0f / inertia : 0.0f,

        .position = position,
        .velocity = Vector2Zero(),
        .acceleration = Vector2Zero(),

        .rotation = 0.0f,
        .angular_velocity = 0.0f,
        .angular_acceleration = 0.0f,

        .restitution = BALL_RESTITUTION,
        .friction = BALL_FRICTION,

        // Debug
        .is_colliding = false,
    };
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
