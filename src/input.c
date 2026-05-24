#include "input.h"
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>

void input_reset(input_t *input)
{
    input->mouse_position = Vector2Zero();

    input->mouse_left_pressed = false;
    input->mouse_left_released = false;

    input->key_space_pressed = false;
    input->key_c_pressed = false;
    input->key_esc_pressed = false;
}

void input_update(input_t *input)
{
    input->mouse_position = GetMousePosition();

    input->mouse_left_pressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    input->mouse_left_released = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

    input->key_space_pressed = IsKeyPressed(KEY_SPACE);
    input->key_esc_pressed = IsKeyPressed(KEY_ESCAPE);
    input->key_c_pressed = IsKeyPressed(KEY_C);
}
