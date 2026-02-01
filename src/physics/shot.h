#ifndef SHOT_H
#define SHOT_H

#include <raylib.h>

typedef struct shot {
    Vector2 start;
    Vector2 end;
} shot_t;

shot_t *shot_create(Vector2 start);
Vector2 shot_vector(const shot_t *shot);

#endif
