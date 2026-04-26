#include "textures.h"
#include "raylib.h"
#include <stdlib.h>

void textures_setup(textures_t *textures)
{
    textures->planets[0] = LoadTexture("textures/mercury.png");
    textures->planets[1] = LoadTexture("textures/venus.png");
    textures->planets[2] = LoadTexture("textures/earth.png");
    textures->planets[3] = LoadTexture("textures/mars.png");
    textures->planets[4] = LoadTexture("textures/jupiter.png");
    textures->planets[5] = LoadTexture("textures/saturn.png");
    textures->planets[6] = LoadTexture("textures/uranus.png");
    textures->planets[7] = LoadTexture("textures/neptune.png");
    textures->planets[8] = LoadTexture("textures/pluto.png");

    textures->hole = LoadTexture("textures/black-hole.png");
}

Texture2D get_planet_texture(const textures_t *textures, const planet_t planet)
{
    switch (planet) {
    case MERCURY:
        return textures->planets[0];
    case VENUS:
        return textures->planets[1];
    case EARTH:
        return textures->planets[2];
    case MARS:
        return textures->planets[3];
    case JUPITER:
        return textures->planets[4];
    case SATURN:
        return textures->planets[5];
    case URANUS:
        return textures->planets[6];
    case NEPTUNE:
        return textures->planets[7];
    case PLUTO:
        return textures->planets[8];
    case NUM_PLANETS: // Just to satisfy the compiler
        return textures->planets[NUM_PLANETS - 1];
    }
}
