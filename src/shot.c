#include "shot.h"
#include "constants.h"
#include "physics/planets.h"
#include "raymath.h"
#include <stdlib.h>
#include <string.h>

shot_t *shot_create(Vector2 start)
{
    shot_t *shot = malloc(sizeof(shot_t));
    shot->start = start;
    shot->end = start;
    shot->end_changed = true;

    for (int i = 0; i < NUM_PLANETS; i++) {
        shot->trajectories[i].planet = (planet_t) i;
        shot->trajectories[i].positions_length = 0;
    }

    return shot;
}

void shot_update_end(shot_t *shot, Vector2 mouse_position)
{
    if (Vector2DistanceSqr(shot->end, mouse_position) < SHOT_PREVIEW_MIN_CHANGE)
        shot->end_changed = false;
    else {
        shot->end = mouse_position;
        shot->end_changed = true;
    }
}

Vector2 shot_vector(const shot_t *shot)
{
    Vector2 raw_shot_vec = Vector2Subtract(shot->start, shot->end);
    float length = Vector2Length(raw_shot_vec);
    if (length > SHOT_MAX_LENGTH) {
        Vector2 normalized = Vector2Normalize(raw_shot_vec);
        return Vector2Scale(normalized, SHOT_MAX_LENGTH);
    } else {
        return raw_shot_vec;
    }
}

void shot_apply(shot_t *shot, world_t *world)
{
    world->gravity_enabled = true;
    ball_t *ball = &world->balls[0];

    Vector2 shot_vec = shot_vector(shot);
    ball_reset_impulses(ball);
    ball_apply_impulse_linear(ball,
                              Vector2Scale(shot_vec, SHOT_IMPULSE_MULTIPLIER));
}

void shot_compute_trajectories(shot_t *shot, world_t *world)
{
    if (!shot->end_changed)
        return;

    for (int i = 0; i < ARR_LEN(shot->trajectories); i++) {
        shot->trajectories[i].positions_length = 0;
    }

    world_t world_cpy;

    memcpy(&world_cpy, world, sizeof(world_t));

    shot_apply(shot, &world_cpy);
    for (int i = 0; i < TRAJECTORY_SIMULATION_STEPS; i++) {
        world_update(&world_cpy, PHYSICS_TIME_STEP);
        if (i % TRAJECTORY_RENDER_EVERY != 0)
            continue;

        bool should_end_simulation = true;
        for (int b = 0; b < world_cpy.balls_count; b++) {
            ball_t *ball = &world_cpy.balls[b];
            if (ball->planet == EARTH) {
                should_end_simulation = false;
            }
            trajectory_t *trajectory = &shot->trajectories[ball->planet];

            if (trajectory->positions_length < ARR_LEN(trajectory->positions)) {
                trajectory->positions[trajectory->positions_length++] =
                    ball->position;
            }
        }

        if (should_end_simulation || world_cpy.balls_count == 0)
            break;
    }

    shot->end_changed = false;
}
