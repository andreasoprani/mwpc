#include "shot.h"
#include "constants.h"
#include "raymath.h"
#include <stdlib.h>

shot_t *shot_create(Vector2 start)
{
    shot_t *shot = malloc(sizeof(shot_t));
    shot->start = start;
    shot->end = start;

    return shot;
}

Vector2 shot_vector(const shot_t *shot)
{
    Vector2 raw_shot_vec = Vector2Subtract(shot->start, shot->end);
    float length = Vector2Length(raw_shot_vec);
    if (length > SHOT_MAX_LENGTH) {
        Vector2 normalized = Vector2Normalize(raw_shot_vec);
        return Vector2Scale(normalized, SHOT_MAX_LENGTH);
    } else {
        return raw_shot_vec;
    }
}
