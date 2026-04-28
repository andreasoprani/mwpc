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

world_t *world_create();
void world_update(world_t *world, const float dt);

void world_place_all_balls(world_t *world);

void world_toggle_gravity(world_t *world);

#endif
