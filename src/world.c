#include "world.h"
#include <stdlib.h>
#include <stdio.h>

const int BODIES_CAPACITY_INCREMENT = 100;

World* createWorld() {
    World* world = (World*)malloc(sizeof(World));
    world->bodies = (Body*)malloc(BODIES_CAPACITY_INCREMENT * sizeof(Body));
    world->bodiesCapacity = BODIES_CAPACITY_INCREMENT;
    world->bodiesLength = 0;
    return world;
};

void destroyWorld(World* world) {
    free(world->bodies);
    free(world);
}

void worldAddBody(World* world, Body* body) {
    if (world->bodiesLength == world->bodiesCapacity) {
        world->bodiesCapacity += BODIES_CAPACITY_INCREMENT;
        world->bodies = (Body*)realloc(world->bodies, world->bodiesCapacity * sizeof(Body));
    }
    world->bodies[world->bodiesLength++] = *body;
}
