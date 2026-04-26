#ifndef PHYSICS_PLANETS_H
#define PHYSICS_PLANETS_H

typedef enum {
    MERCURY,
    VENUS,
    EARTH,
    MARS,
    JUPITER,
    SATURN,
    URANUS,
    NEPTUNE,
    PLUTO,
    NUM_PLANETS
} planet_t;

#define PLANET_BIT(p) (1u << (p))

const char *get_planet_name(const planet_t planet);
float get_planet_mass(const planet_t planet);
float get_planet_radius(const planet_t planet);

#endif
