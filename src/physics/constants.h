#ifndef PHYSICS_CONSTANTS_H
#define PHYSICS_CONSTANTS_H

#define CROSS(a, b) ((a).x * (b).y - (a).y * (b).x)
#define ARR_LEN(arr) (sizeof(arr) / sizeof(*arr))

#define CONTACT_BETA 0.6f
#define CONTACT_SOLVE_ITERATIONS 10

#define CONTACT_POSITION_CORRECTION 0.8f

#define GRAVITATIONAL_CONSTANT 1000.0f

#define DEFAULT_BALL_RESTITUTION 0.9f
#define DEFAULT_BALL_FRICTION 0.7f

#define EARTH_MASS 100.0f
#define EARTH_RADIUS 10.0f

#define MASS_SQUASH_POW_EXP 0.01f
#define RADIUS_SQUASH_POW_EXP 0.4f

#define HOLE_RADIUS 30.0f
#define HOLE_OFFSET_CORNER                                                     \
    0.0f // corner holes sit on the anchor (diagonal outward)
#define HOLE_OFFSET_SIDE                                                       \
    (HOLE_RADIUS * 0.5f) // side holes pushed further out so less of them is
                         // exposed inside the table
#define HOLE_MOUTH_CORNER                                                      \
    (HOLE_RADIUS * 1.0f) // wall cutback from table corner along wall
#define HOLE_MOUTH_SIDE                                                        \
    (HOLE_RADIUS * 1.0f) // wall cutback from table mid-edge along wall
#define JAW_FLARE_ANGLE                                                        \
    40.0f // degrees; must satisfy tan(angle) < HOLE_MOUTH_SIDE/WALL_THICKNESS

#define DEFAULT_WALL_RESTITUTION 0.9f // [0, 1]
#define DEFAULT_WALL_FRICTION 0.7f    // [0, 1]
#define WALL_THICKNESS 20.0f

#define SHOT_IMPULSE_MULTIPLIER 500.0f // J
#define SHOT_MAX_LENGTH 150.0f         // In pixels

#endif
