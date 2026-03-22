#include "textures.h"
#include "physics/ball.h"
#include "raylib.h"
#include <stdlib.h>

textures_t *textures_setup()
{
    textures_t *textures = malloc(sizeof(textures_t));

    textures->planets[0] = LoadTexture("textures/mercury.png");
    textures->planets[1] = LoadTexture("textures/venus.png");
    textures->planets[2] = LoadTexture("textures/earth.png");
    textures->planets[3] = LoadTexture("textures/mars.png");
    textures->planets[4] = LoadTexture("textures/jupiter.png");
    textures->planets[5] = LoadTexture("textures/saturn.png");
    textures->planets[6] = LoadTexture("textures/uranus.png");
    textures->planets[7] = LoadTexture("textures/neptune.png");
    textures->planets[8] = LoadTexture("textures/pluto.png");

    return textures;
}

Texture2D get_planet_texture(const textures_t *textures, const planet_t planet)
{
    // TODO: probably need to change the whole NUM_PLANETS logic to avoid this
    // wrap around and just never have more balls than sprites
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
    }
}
