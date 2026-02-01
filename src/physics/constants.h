#ifndef PHYSICS_CONSTANTS_H
#define PHYSICS_CONSTANTS_H

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define CROSS(a, b) ((a).x * (b).y - (a).y * (b).x)

#define CONTACT_BETA 0.6f
#define CONTACT_SOLVE_ITERATIONS 10

#define GRAVITATIONAL_CONSTANT 10000

#define DEFAULT_BALL_RESTITUTION 0.9f
#define DEFAULT_BALL_FRICTION 0.7f
#define BALL_DEFAULT_RADIUS 20.0f
#define BALL_DEFAULT_MASS 100.0f

#define DEFAULT_WALL_RESTITUTION 0.9f // [0, 1]
#define DEFAULT_WALL_FRICTION 0.7f    // [0, 1]

#define SHOT_IMPULSE_MULTIPLIER 500.0f // J
#define SHOT_MAX_LENGTH 150.0f         // In pixels

#endif
