#include "ball.h"
#include "raymath.h"
#include <stdlib.h>

const float DEFAULT_BALL_RESTITUTION = 0.9f;

Ball *ball_create(const Vector2 position, const float radius,
                  const float mass) {
  Ball *ball = malloc(sizeof(Ball));

  ball->radius = radius;

  ball->position = position;
  ball->velocity = Vector2Zero();
  ball->acceleration = Vector2Zero();

  ball->rotation = 0.f;
  ball->angularVelocity = 0.f;
  ball->angularAcceleration = 0.f;

  ball->mass = mass >= 0 ? mass : 0;
  if (mass > 0)
    ball->inverseMass = 1.f / ball->mass;
  else
    ball->inverseMass = 0.f;

  ball->inertia = 0.5 * radius * radius * mass;
  if (ball->inertia > 0)
    ball->inverseInertia = 1.f / ball->inertia;
  else
    ball->inverseInertia = 0.f;

  ball->restitution = DEFAULT_BALL_RESTITUTION;

  return ball;
}

int ball_is_static(const Ball *ball) {
  const float epsilon = 0.005f;
  return fabs(ball->inverseMass) < epsilon;
}

void ball_add_force(Ball *ball, const Vector2 force) {
  ball->sumForces = Vector2Add(ball->sumForces, force);
}

void ball_add_torque(Ball *ball, const float torque) {
  ball->sumTorques += torque;
}

void ball_clear_forces(Ball *ball) {
  ball->sumForces = Vector2Zero();
  ball->sumTorques = 0;
}

void ball_apply_impulse_linear(Ball *ball, const Vector2 j) {
  if (ball_is_static(ball))
    return;

  ball->velocity =
      Vector2Add(ball->velocity, Vector2Scale(j, ball->inverseMass));
}

void ball_apply_impulse_angular(Ball *ball, const float j) {
  if (ball_is_static(ball))
    return;

  ball->angularVelocity += j * ball->inverseInertia;
}

void ball_apply_impulse_at_point(Ball *ball, const Vector2 j, const Vector2 r) {
  if (ball_is_static(ball))
    return;

  ball->velocity =
      Vector2Add(ball->velocity, Vector2Scale(j, ball->inverseMass));
  ball->angularVelocity += (r.x * j.y - r.y * j.x) * ball->inverseInertia;
}

void ball_integrate_force(Ball *ball, const float dt) {
  if (ball_is_static(ball))
    return;

  ball->acceleration = Vector2Scale(ball->sumForces, ball->inverseMass);
  ball->angularAcceleration = ball->sumTorques * ball->inverseInertia;

  ball->velocity =
      Vector2Add(ball->velocity, Vector2Scale(ball->acceleration, dt));
  ball->rotation += ball->angularVelocity * dt;

  ball_clear_forces(ball);
}

void ball_integrate_velocity(Ball *ball, const float dt) {
  if (ball_is_static(ball))
    return;

  ball->position = Vector2Add(ball->position, Vector2Scale(ball->velocity, dt));
  ball->rotation += ball->angularVelocity * dt;
}

Vector2 ballLocalSpaceToWorldSpace(Ball *ball, const Vector2 localPosition) {
  return Vector2Add(ball->position,
                    Vector2Rotate(localPosition, ball->rotation));
}
Vector2 ballWorldSpaceToLocalSpace(Ball *ball, const Vector2 worldPosition) {
  return Vector2Rotate(Vector2Subtract(worldPosition, ball->position),
                       -ball->rotation);
}

bool balls_are_colliding(Ball *ball1, Ball *ball2, BallsContact *contact) {
  const Vector2 ab = Vector2Subtract(ball2->position, ball1->position);
  const float radiusSum = ball1->radius + ball2->radius;
  const bool isColliding = Vector2LengthSqr(ab) <= radiusSum * radiusSum;

  if (!isColliding)
    return false;

  BallsContact newContact;

  newContact.ball1 = ball1;
  newContact.ball2 = ball2;
  newContact.normal = Vector2Normalize(ab);
  newContact.start = Vector2Subtract(
      ball2->position, Vector2Scale(newContact.normal, ball2->radius));
  newContact.depth = radiusSum - Vector2Length(ab);

  *contact = newContact;

  return true;
}
