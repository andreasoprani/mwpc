#ifndef INPUT_H
#define INPUT_H

#include "raylib.h"

typedef struct input {
    Vector2 mouse_position;

    bool mouse_left_pressed;

    bool key_d_pressed;
    bool key_g_pressed;
    bool key_space_pressed;
} input_t;

input_t *input_create();
void input_reset(input_t *input);
void input_update(input_t *input);

#endif
