#ifndef BODY_H
#define BODY_H

#include <raylib.h>
#include <raymath.h>

typedef struct Body {
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
} Body;

Body *bodyCreate(Vector2 position, float mass);

int bodyIsStatic(const Body *body);

void bodyAddForce(Body *body, Vector2 force);
void bodyAddTorque(Body *body, float torque);
void bodyClearForces(Body *body);

void bodyIntegrateForces(Body *body, const float dt);
void bodyIntegrateVelocity(Body *body, const float dt);

#endif
