#ifndef INPUT_H
#define INPUT_H

#include "raylib.h"

typedef struct Input {
    Vector2 mousePosition;

    bool mouseLeftPressed;

    bool keyDPressed;
    bool keyGPressed;
    bool keySpacePressed;
} Input;

Input *input_create();
void input_reset(Input *input);
void input_update(Input *input);

#endif
