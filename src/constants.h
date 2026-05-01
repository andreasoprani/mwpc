#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "physics/constants.h"
#include "raylib.h"

#define TABLE_H_PAD_RATIO 0.05f

#define CONTROLLED_BALL_POSITION                                               \
    (Vector2){TABLE_WIDTH * 0.5f, TABLE_HEIGHT * 0.75f}
#define BALLS_APEX (Vector2){TABLE_WIDTH * 0.5f, TABLE_HEIGHT * 0.25f}
#define CONE_ANGLE 75.0f // degrees
#define BALLS_GAP 0.2f

#endif
