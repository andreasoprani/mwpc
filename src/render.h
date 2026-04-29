#ifndef RENDER_H
#define RENDER_H

#include "app.h"
#include "raylib.h"
#include "world.h"

Vector2 render_screen_to_world(const world_t *world, Vector2 screen_position);
void render(const app_t *app);

#endif
