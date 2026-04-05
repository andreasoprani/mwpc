#include "contact.h"
#include "ball.h"
#include "constants.h"
#include "matmath.h"
#include "raylib.h"
#include "raymath.h"
#include "table.h"
#include <float.h>
#include <stdio.h>
#include <string.h>

bool balls_are_colliding(ball_t *ball1, ball_t *ball2, contact_t *contact)
{
    const Vector2 ab = Vector2Subtract(ball2->position, ball1->position);
    const float radius_sum = ball1->radius + ball2->radius;
    const bool is_colliding = Vector2LengthSqr(ab) <= radius_sum * radius_sum;

    if (!is_colliding)
        return false;

    contact->type = CONTACT_BALL_BALL;
    contact->ball = ball1;
    contact->other.ball = ball2;
    contact->normal = Vector2Normalize(ab);
    contact->start = Vector2Subtract(
        ball2->position, Vector2Scale(contact->normal, ball2->radius));
    contact->end = Vector2Add(ball1->position,
                              Vector2Scale(contact->normal, ball1->radius));
    contact->depth =
        Vector2Length(Vector2Subtract(contact->end, contact->start));

    init_contact_resolution_info(contact);

    return true;
}

bool ball_wall_are_colliding(ball_t *ball, wall_t *wall, contact_t *contact)
{
    bool is_outside = false;
    Vector2 min_curr = {0}, min_next = {0};
    float max_proj = -FLT_MAX;

    for (int i = 0; i < wall->num_vertices; i++) {
        Vector2 curr = wall->vertices[i];
        Vector2 next = wall->vertices[(i + 1) % wall->num_vertices];
        Vector2 edge = Vector2Subtract(next, curr);
        // Outward normal for CW polygon: (edge.y, -edge.x)
        Vector2 normal = Vector2Normalize((Vector2) {edge.y, -edge.x});

        float proj =
            Vector2DotProduct(Vector2Subtract(ball->position, curr), normal);
        if (proj > 0) {
            max_proj = proj;
            min_curr = curr;
            min_next = next;
            is_outside = true;
            break;
        }
        if (proj > max_proj) {
            max_proj = proj;
            min_curr = curr;
            min_next = next;
        }
    }

    Vector2 edge = Vector2Subtract(min_next, min_curr);
    // wall_normal points from polygon surface toward ball
    Vector2 wall_normal;
    float depth;

    if (is_outside) {
        Vector2 curr_to_ball = Vector2Subtract(ball->position, min_curr);
        if (Vector2DotProduct(curr_to_ball, edge) < 0.f) {
            // Region A: nearest to start vertex
            float dist = Vector2Length(curr_to_ball);
            depth = ball->radius - dist;
            if (depth < 0.f)
                return false;
            wall_normal = Vector2Normalize(curr_to_ball);
        } else {
            Vector2 next_to_ball = Vector2Subtract(ball->position, min_next);
            if (Vector2DotProduct(next_to_ball, Vector2Negate(edge)) < 0.f) {
                // Region B: nearest to end vertex
                float dist = Vector2Length(next_to_ball);
                depth = ball->radius - dist;
                if (depth < 0.f)
                    return false;
                wall_normal = Vector2Normalize(next_to_ball);
            } else {
                // Region C: nearest to edge face
                depth = ball->radius - max_proj;
                if (depth < 0.f)
                    return false;
                wall_normal = Vector2Normalize((Vector2) {edge.y, -edge.x});
            }
        }
    } else {
        // Ball center is inside polygon
        depth = ball->radius - max_proj;
        wall_normal = Vector2Normalize((Vector2) {edge.y, -edge.x});
    }

    contact->type = CONTACT_BALL_WALL;
    contact->ball = ball;
    contact->other.wall = wall;
    contact->normal = Vector2Negate(wall_normal);
    contact->depth = depth;
    contact->start =
        Vector2Add(ball->position, Vector2Scale(contact->normal, ball->radius));
    contact->end =
        Vector2Add(contact->start, Vector2Scale(contact->normal, depth));

    init_contact_resolution_info(contact);

    return true;
}

void init_contact_resolution_info(contact_t *contact)
{
    memset(&contact->jacobian, 0, sizeof contact->jacobian);
    memset(&contact->cached_lambda, 0, sizeof contact->cached_lambda);
}

void contact_pre_solve(contact_t *contact, float dt)
{
    const contact_type_t type = contact->type;
    const Vector2 n = contact->normal;

    const Vector2 r_ball =
        Vector2Subtract(contact->start, contact->ball->position);

    Vector2 r_other;
    if (type == CONTACT_BALL_BALL) {
        r_other = Vector2Subtract(contact->end, contact->other.ball->position);
    } else {
        r_other = Vector2Zero();
    }

    contact->jacobian[0] = -n.x;
    contact->jacobian[1] = -n.y;
    contact->jacobian[2] = -CROSS(r_ball, n);

    contact->jacobian[3] = n.x;
    contact->jacobian[4] = n.y;
    contact->jacobian[5] = CROSS(r_other, n);

    const float friction = get_contact_friction(contact);
    if (friction > 0.f) {
        Vector2 t = {n.y, -n.x};
        contact->jacobian[6] = -t.x;
        contact->jacobian[7] = -t.y;
        contact->jacobian[8] = -CROSS(r_ball, t);
        contact->jacobian[9] = t.x;
        contact->jacobian[10] = t.y;
        contact->jacobian[11] = CROSS(r_other, t);
    }

    float e = get_contact_restitution(contact);
    Vector2 w_ball = {-contact->ball->angular_velocity * r_ball.y,
                      contact->ball->angular_velocity * r_ball.x};
    Vector2 v_ball = Vector2Add(contact->ball->velocity, w_ball);

    Vector2 v_other;
    if (type == CONTACT_BALL_BALL) {
        const Vector2 w_other = {
            -contact->other.ball->angular_velocity * r_other.y,
            contact->other.ball->angular_velocity * r_other.x};
        v_other = Vector2Add(contact->other.ball->velocity, w_other);
    } else {
        v_other = Vector2Zero();
    }

    contact->bias = -e * Vector2DotProduct(Vector2Subtract(v_ball, v_other), n);
}

void contact_solve(contact_t *contact, float dt)
{
    float *j = contact->jacobian;

    float jT[6 * 2]; // NOLINT(readability-identifier-naming)
    mat_transpose(j, jT, 2, 6);

    float velocities[6];
    get_contact_velocities_vector(contact, velocities);

    float invM[6 * 6]; // NOLINT(readability-identifier-naming)
    get_contact_invm_matrix(contact, invM);

    float tmp[2 * 6];
    mat_mul_mat(j, invM, tmp, 2, 6, 6);
    float lhs[2 * 2];
    mat_mul_mat(tmp, jT, lhs, 2, 6, 2);
    float rhs[2];
    mat_mul_vec(j, velocities, rhs, 2, 6);
    vec_scale(rhs, rhs, -1, 2);
    rhs[0] -= contact->bias;

    float lambda[2] = {0.f, 0.f};
    solve_gauss_seidel(lhs, rhs, lambda, 2);

    float old_lambda[2];
    vec_copy(contact->cached_lambda, old_lambda, 2);

    vec_add(contact->cached_lambda, lambda, contact->cached_lambda, 2);
    contact->cached_lambda[0] = fmaxf(contact->cached_lambda[0], 0.f);

    const float friction = get_contact_friction(contact);
    if (friction > 0.f) {
        const float max_friction = contact->cached_lambda[0] * friction;
        contact->cached_lambda[1] =
            fminf(contact->cached_lambda[1], max_friction);
        contact->cached_lambda[1] =
            fmaxf(contact->cached_lambda[1], -max_friction);
    }

    vec_sub(contact->cached_lambda, old_lambda, lambda, 2);

    float impulses[6];
    mat_mul_vec(jT, lambda, impulses, 6, 2);

    Vector2 il1 = {impulses[0], impulses[1]};
    float ia1 = impulses[2];
    Vector2 il2 = {impulses[3], impulses[4]};
    float ia2 = impulses[5];

    ball_apply_impulse_linear(contact->ball, il1);
    ball_apply_impulse_angular(contact->ball, ia1);

    if (contact->type == CONTACT_BALL_BALL) {
        ball_apply_impulse_linear(contact->other.ball, il2);
        ball_apply_impulse_angular(contact->other.ball, ia2);
    }
}

void contact_correct_position(contact_t *contact, float dt)
{
    if (contact->depth <= 0)
        return;

    float total_inv_mass =
        contact->ball->inverse_mass + (contact->type == CONTACT_BALL_BALL
                                           ? contact->other.ball->inverse_mass
                                           : 0.0f);
    if (total_inv_mass == 0.f)
        return;

    Vector2 correction = Vector2Scale(
        contact->normal,
        contact->depth * CONTACT_POSITION_CORRECTION / total_inv_mass);

    contact->ball->position =
        Vector2Subtract(contact->ball->position,
                        Vector2Scale(correction, contact->ball->inverse_mass));
    if (contact->type == CONTACT_BALL_BALL)
        contact->other.ball->position = Vector2Add(
            contact->other.ball->position,
            Vector2Scale(correction, contact->other.ball->inverse_mass));
}

float get_contact_friction(contact_t *contact)
{
    float second_friction;
    if (contact->type == CONTACT_BALL_BALL)
        second_friction = contact->other.ball->friction;
    else
        second_friction = contact->other.wall->friction;
    return fmaxf(contact->ball->friction, second_friction);
}

float get_contact_restitution(contact_t *contact)
{
    float second_restitution;
    if (contact->type == CONTACT_BALL_BALL)
        second_restitution = contact->other.ball->restitution;
    else
        second_restitution = contact->other.wall->restitution;
    return fminf(contact->ball->restitution, second_restitution);
}

void get_contact_velocities_vector(contact_t *contact, float *velocities)
{
    vec_zero(velocities, 6);

    velocities[0] = contact->ball->velocity.x;
    velocities[1] = contact->ball->velocity.y;
    velocities[2] = contact->ball->angular_velocity;

    if (contact->type == CONTACT_BALL_BALL) {
        velocities[3] = contact->other.ball->velocity.x;
        velocities[4] = contact->other.ball->velocity.y;
        velocities[5] = contact->other.ball->angular_velocity;
    }
}

void get_contact_invm_matrix(contact_t *contact, float *invM)
{
    mat_zero(invM, 6, 6);

    invM[mat_idx(6, 0, 0)] = contact->ball->inverse_mass;
    invM[mat_idx(6, 1, 1)] = contact->ball->inverse_mass;
    invM[mat_idx(6, 2, 2)] = contact->ball->inverse_inertia;

    if (contact->type == CONTACT_BALL_BALL) {
        invM[mat_idx(6, 3, 3)] = contact->other.ball->inverse_mass;
        invM[mat_idx(6, 4, 4)] = contact->other.ball->inverse_mass;
        invM[mat_idx(6, 5, 5)] = contact->other.ball->inverse_inertia;
    }
}
