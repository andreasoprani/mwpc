#include "app.h"
#include "raylib.h"
#include <stdlib.h>

int main() {
  App *app = appSetup();

  while (WindowShouldClose() == false) {
    appFrame(app);
  }

  CloseWindow();

  return 0;
}
