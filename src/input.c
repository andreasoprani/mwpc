#include "input.h"
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>

void input_reset(input_t *input)
{
    input->mouse_position = Vector2Zero();

    input->mouse_left_pressed = false;
    input->mouse_left_released = false;

    input->touch_down = false;
    input->touch_pressed = false;
    input->touch_released = false;

    input->key_space_pressed = false;
    input->key_c_pressed = false;
    input->key_esc_pressed = false;
}

void input_update(input_t *input)
{
    const int touch_count = GetTouchPointCount();
    const bool touch_down = touch_count > 0;
    input->touch_pressed = touch_down && !input->touch_down;
    input->touch_released = !touch_down && input->touch_down;
    input->touch_down = touch_down;

    input->mouse_position =
        touch_down ? GetTouchPosition(0) : GetMousePosition();

    input->mouse_left_pressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    input->mouse_left_released = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

    input->key_space_pressed = IsKeyPressed(KEY_SPACE);
    input->key_esc_pressed = IsKeyPressed(KEY_ESCAPE);
    input->key_c_pressed = IsKeyPressed(KEY_C);
}
