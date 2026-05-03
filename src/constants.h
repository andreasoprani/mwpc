#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "physics/constants.h"
#include "raylib.h"

#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 720
#define MAX_FRAME_TIME (1.0f / 30.0f)
#define PHYSICS_FRAME_RATE 120
#define PHYSICS_TIME_STEP (1.0f / (float) PHYSICS_FRAME_RATE)

#define TABLE_H_PAD_RATIO 0.05f

#define CONTROLLED_BALL_POSITION                                               \
    (Vector2){TABLE_WIDTH * 0.5f, TABLE_HEIGHT * 0.75f}
#define BALLS_APEX (Vector2){TABLE_WIDTH * 0.5f, TABLE_HEIGHT * 0.25f}
#define CONE_ANGLE 75.0f // degrees
#define BALLS_GAP 0.2f

#define TRAJECTORY_PREVIEW_NUMERATOR 1
#define TRAJECTORY_PREVIEW_DENOMINATOR 4

#define TRAJECTORY_SIMULATION_STEPS                                            \
    ((int) PHYSICS_FRAME_RATE * TRAJECTORY_PREVIEW_NUMERATOR /                 \
     TRAJECTORY_PREVIEW_DENOMINATOR)
#define TRAJECTORY_RENDER_EVERY 2
#define SHOT_PREVIEW_MIN_CHANGE 0.0f

#define SHOT_MAX_RADIUS_HEIGHT_FRACTION 0.1f

#endif
