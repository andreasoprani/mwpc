#ifndef WORLD_H
#define WORLD_H

#include "physics/ball.h"
#include "physics/contact.h"
#include "physics/planets.h"
#include "physics/table.h"
#include "textures.h"

typedef struct world {
    ball_t balls[NUM_PLANETS];
    unsigned int balls_count;

    table_t table;

    contact_t
        contacts[NUM_PLANETS * (NUM_PLANETS - 1) + NUM_PLANETS * NUM_WALLS];
    unsigned int contacts_count;

    bool gravity_enabled;
} world_t;

void world_setup(world_t *world, Vector2 table_origin, float table_width,
                 float table_height);
void world_destroy(world_t *world);
void world_update(world_t *world, const float dt);

void world_place_all_balls(world_t *world, const planet_t controlled_planet,
                           const planet_t *other_planets,
                           const unsigned int num_other_planets);

void world_toggle_gravity(world_t *world);

#endif
