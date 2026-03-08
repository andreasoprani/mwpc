#ifndef TEXTURES_H
#define TEXTURES_H

#include "raylib.h"

#define NUM_PLANETS 9

typedef struct textures {
    Texture2D planets[NUM_PLANETS];
} textures_t;

textures_t *textures_setup();

Texture2D get_planet_texture(const textures_t *textures, const unsigned int id);

#endif
