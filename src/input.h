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

Input *inputCreate();
void inputReset(Input *input);
void inputUpdate(Input *input);

#endif
