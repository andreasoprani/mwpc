#ifndef BODY_H
#define BODY_H

#include <raymath.h>

typedef struct Body {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;

    float rotation;
    float angularVelocity;
    float angularAcceleration;

    float mass;
    float inverseMass;

    float inertia;
    float inverseInertia;

    float restitution;
} Body;

Body* createBody(Vector2 position, float mass);


#endif
