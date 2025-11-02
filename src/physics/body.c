#include "body.h"
#include "raymath.h"
#include <stdlib.h>

Body *bodyCreate(Vector2 position, float mass) {
  Body *body = malloc(sizeof(Body));

  body->position = position;
  body->velocity = Vector2Zero();
  body->acceleration = Vector2Zero();

  body->rotation = 0.f;
  body->angularVelocity = 0.f;
  body->angularAcceleration = 0.f;

  body->mass = mass >= 0 ? mass : 0;
  if (mass > 0)
    body->inverseMass = 1.f / body->mass;
  else
    body->inverseMass = 0.f;

  // TODO
  body->inertia = 0.f;
  body->inverseInertia = 0.f;

  // TODO
  body->restitution = 1.f;

  return body;
}

int bodyIsStatic(const Body *body) {
  const float epsilon = 0.005f;
  return fabs(body->inverseMass) < epsilon;
}

void bodyAddForce(Body *body, const Vector2 force) {
  body->sumForces = Vector2Add(body->sumForces, force);
}

void bodyAddTorque(Body *body, const float torque) {
  body->sumTorques += torque;
}

void bodyClearForces(Body *body) {
  body->sumForces = Vector2Zero();
  body->sumTorques = 0;
}

void bodyIntegrateForces(Body *body, const float dt) {
  if (bodyIsStatic(body))
    return;

  body->acceleration = Vector2Scale(body->sumForces, body->inverseMass);
  body->angularAcceleration = body->sumTorques * body->inverseInertia;

  body->velocity =
      Vector2Add(body->velocity, Vector2Scale(body->acceleration, dt));
  body->rotation += body->angularVelocity * dt;

  bodyClearForces(body);
}

void bodyIntegrateVelocity(Body *body, const float dt) {
  if (bodyIsStatic(body))
    return;

  body->position = Vector2Add(body->position, Vector2Scale(body->velocity, dt));
  body->rotation += body->angularVelocity * dt;
}
