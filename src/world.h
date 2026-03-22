#ifndef WORLD_H
#define WORLD_H

#include "physics/ball.h"
#include "physics/contact.h"
#include "physics/planets.h"
#include "physics/wall.h"

typedef struct world {
    ball_t **balls;
    unsigned int balls_capacity;
    unsigned int balls_length;

    table_t *table;

    contact_t *contacts;
    unsigned int contacts_capacity;
    unsigned int contacts_length;

    bool gravity_enabled;
} world_t;

world_t *world_create(table_t *table);
void world_destroy(world_t *world);
void world_update(world_t *world, const float dt);

void world_add_ball(world_t *world, ball_t *ball);
void world_place_all_balls(world_t *world, const planet_t controlled_planet,
                           const planet_t *other_planets,
                           const unsigned int num_other_planets);

void world_toggle_gravity(world_t *world);

#endif
