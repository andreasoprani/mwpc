#ifndef PHYSICS_CONSTANTS_H
#define PHYSICS_CONSTANTS_H

#define CROSS(a, b) ((a).x * (b).y - (a).y * (b).x)
#define ARR_LEN(arr) (sizeof(arr) / sizeof(*arr))

#define CONTACT_BETA 0.6f
#define CONTACT_SOLVE_ITERATIONS 10

#define CONTACT_POSITION_CORRECTION 0.8f

#define TABLE_RATIO 2.0f
#define WORLD_TABLE_HEIGHT 100.0f
#define WORLD_TABLE_WIDTH (WORLD_TABLE_HEIGHT / TABLE_RATIO)

#define GRAVITATIONAL_CONSTANT 200.0f

#define BALL_RESTITUTION 0.9f
#define BALL_FRICTION 0.7f

#define EARTH_MASS 10.0f
#define EARTH_RADIUS 2.0f

#define MASS_SQUASH_POW_EXP 0.01f
#define RADIUS_SQUASH_POW_EXP 0.15f

#define HOLE_RADIUS 6.0f
#define HOLE_ROTATION_SPEED 0.02f

// corner holes sit on the anchor (diagonal outward)
#define HOLE_OFFSET_CORNER 0.0f
// side holes pushed further out so less of them is
#define HOLE_OFFSET_SIDE (HOLE_RADIUS * 0.5f)

// exposed inside the table
// wall cutback from table corner along wall
#define HOLE_MOUTH_CORNER (HOLE_RADIUS * 1.0f)
// wall cutback from table mid-edge along wall
#define HOLE_MOUTH_SIDE (HOLE_RADIUS * 1.3f)
// degrees; must satisfy tan(angle) < HOLE_MOUTH_SIDE/WALL_THICKNESS
#define JAW_FLARE_ANGLE 40.0f

#define WALL_RESTITUTION 0.9f // [0, 1]
#define WALL_FRICTION 0.7f    // [0, 1]
#define WALL_THICKNESS 4.0f

#define SHOT_IMPULSE_MULTIPLIER 100.0f // J
#define SHOT_MAX_LENGTH 30.0f          // In pixels

#endif
