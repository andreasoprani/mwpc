#include "planets.h"
#include "constants.h"
#include <math.h>

#define SQUASH_POW_MASS(ratio) powf(ratio, MASS_SQUASH_POW_EXP)
#define SQUASH_POW_RADIUS(ratio) powf(ratio, RADIUS_SQUASH_POW_EXP)

static const char *const PLANET_NAMES[] = {
    [MERCURY] = "Mercury", [VENUS] = "Venus",     [EARTH] = "Earth",
    [MARS] = "Mars",       [JUPITER] = "Jupiter", [SATURN] = "Saturn",
    [URANUS] = "Uranus",   [NEPTUNE] = "Neptune", [PLUTO] = "Pluto",
};

static const float MASS_RATIOS[] = {
    [MERCURY] = 0.055f, [VENUS] = 0.82f,     [EARTH] = 1.0f,
    [MARS] = 0.107f,    [JUPITER] = 1898.6f, [SATURN] = 568.46f,
    [URANUS] = 86.62f,  [NEPTUNE] = 102.43f, [PLUTO] = 0.01f,
};

static const float RADIUS_RATIOS[] = {
    [MERCURY] = 0.383f, [VENUS] = 0.95f,     [EARTH] = 1.0f,
    [MARS] = 0.532f,    [JUPITER] = 11.221f, [SATURN] = 9.46f,
    [URANUS] = 3.92f,   [NEPTUNE] = 3.887f,  [PLUTO] = 0.181f,
};

const char *get_planet_name(const planet_t planet)
{
    return PLANET_NAMES[planet];
}

float get_planet_mass(const planet_t planet)
{
    return SQUASH_POW_MASS(MASS_RATIOS[planet]) * EARTH_MASS;
}

float get_planet_radius(const planet_t planet)
{
    return SQUASH_POW_RADIUS(RADIUS_RATIOS[planet]) * EARTH_RADIUS;
}
