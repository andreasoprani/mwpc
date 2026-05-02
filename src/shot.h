#ifndef SHOT_H
#define SHOT_H

#include "constants.h"
#include "world.h"
#include <raylib.h>

typedef struct trajectory {
    planet_t planet;
    Vector2 positions[TRAJECTORY_SIMULATION_STEPS / TRAJECTORY_RENDER_EVERY];
    unsigned int positions_length;
} trajectory_t;

typedef struct shot {
    Vector2 start;
    Vector2 end;
    bool end_changed;

    trajectory_t trajectories[NUM_PLANETS];
} shot_t;

shot_t *shot_create(Vector2 start);
void shot_update_end(shot_t *shot, Vector2 mouse_position);
Vector2 shot_vector(const shot_t *shot);
void shot_apply(shot_t *shot, world_t *world);
void shot_compute_trajectories(shot_t *shot, world_t *world);

#endif
