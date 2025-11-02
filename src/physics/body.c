#include "body.h"
#include "raymath.h"
#include <stdlib.h>

Body* createBody(Vector2 position, float mass) {
    Body* body = malloc(sizeof(Body));

    body->position = position;
    body->velocity = Vector2Zero();
    body->acceleration = Vector2Zero();

    body->rotation = 0.f;
    body->angularVelocity = 0.f;
    body->angularAcceleration = 0.f;

    body->mass = mass >= 0 ? mass : 0;
    if (mass > 0) body->inverseMass = 1.f / body->mass;
    else body->inverseMass = 0.f;

    // TODO
    body->inertia = 0.f;
    body->inverseInertia = 0.f;

    // TODO
    body->restitution = 1.f;

    return body;
}
