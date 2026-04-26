#ifndef TEXTURES_H
#define TEXTURES_H

#include "physics/ball.h"
#include "raylib.h"

// TODO: hardcode based on planet_t
#define NUM_PLANETS 9

typedef struct textures {
    Texture2D planets[NUM_PLANETS];
    Texture2D hole;
} textures_t;

void textures_setup(textures_t *textures);

Texture2D get_planet_texture(const textures_t *textures, const planet_t planet);

#endif
