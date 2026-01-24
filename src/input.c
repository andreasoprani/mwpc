#include "input.h"
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>

Input *input_create()
{
    Input *input = malloc(sizeof(Input));
    input_reset(input);
    return input;
}

void input_reset(Input *input)
{
    input->mousePosition = Vector2Zero();

    input->mouseLeftPressed = false;

    input->keyDPressed = false;
    input->keyGPressed = false;
    input->keySpacePressed = false;
}

void input_update(Input *input)
{
    input->mousePosition = GetMousePosition();

    input->mouseLeftPressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    input->keyDPressed = IsKeyPressed(KEY_D);
    input->keyGPressed = IsKeyPressed(KEY_G);
    input->keySpacePressed = IsKeyPressed(KEY_SPACE);
}
