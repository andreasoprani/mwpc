#include "contact.h"
#include "ball.h"
#include "matmath.h"
#include "raylib.h"
#include "raymath.h"
#include "wall.h"
#include <stdio.h>
#include <string.h>

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define CROSS(a, b) ((a).x * (b).y - (a).y * (b).x)

const float BETA = 0.2f;

bool balls_are_colliding(Ball *ball1, Ball *ball2, Contact *contact) {
  const Vector2 ab = Vector2Subtract(ball2->position, ball1->position);
  const float radiusSum = ball1->radius + ball2->radius;
  const bool isColliding = Vector2LengthSqr(ab) <= radiusSum * radiusSum;

  if (!isColliding)
    return false;

  contact->type = CONTACT_BALL_BALL;
  contact->ball = ball1;
  contact->other.ball = ball2;
  contact->normal = Vector2Normalize(ab);
  contact->start = Vector2Subtract(
      ball2->position, Vector2Scale(contact->normal, ball2->radius));
  contact->end =
      Vector2Add(ball1->position, Vector2Scale(contact->normal, ball1->radius));
  contact->depth = Vector2Length(Vector2Subtract(contact->end, contact->start));

  init_contact_resolution_info(contact);

  return true;
}

bool ball_wall_are_colliding(Ball *ball, Wall *wall, Contact *contact) {
  const Vector2 start_to_ball = Vector2Subtract(ball->position, wall->start);

  const Vector2 wall_closest_point = Vector2Add(
      wall->start,
      Vector2Scale(wall->direction,
                   Vector2DotProduct(start_to_ball, wall->direction)));

  const Vector2 outside_normal = wall_get_outside_normal(wall);

  const Vector2 wall_closest_point_to_ball_distance =
      Vector2Subtract(ball->position, wall_closest_point);

  const bool insideWall = Vector2DotProduct(wall_closest_point_to_ball_distance,
                                            outside_normal) >= 0.f;

  const bool collidingWall =
      Vector2LengthSqr(wall_closest_point_to_ball_distance) <=
      ball->radius * ball->radius;

  float depth;
  if (insideWall) {
    depth = Vector2Length(wall_closest_point_to_ball_distance) + ball->radius;
  } else if (collidingWall) {
    depth = ball->radius - Vector2Length(wall_closest_point_to_ball_distance);
  } else {
    return false;
  }

  contact->type = CONTACT_BALL_WALL;
  contact->ball = ball;
  contact->other.wall = wall;
  contact->normal = outside_normal;
  contact->start = wall_closest_point;
  contact->end =
      Vector2Add(contact->start, Vector2Scale(contact->normal, depth));
  contact->depth = depth;

  init_contact_resolution_info(contact);

  return true;
}

void init_contact_resolution_info(Contact *contact) {
  memset(&contact->jacobian, 0, sizeof contact->jacobian);
  memset(&contact->cachedLambda, 0, sizeof contact->cachedLambda);
}

void contact_pre_solve(Contact *contact, float dt) {
  const ContactType type = contact->type;
  const Vector2 n = contact->normal;

  const Vector2 rBall =
      Vector2Subtract(contact->start, contact->ball->position);

  Vector2 rOther;
  if (type == CONTACT_BALL_BALL) {
    rOther = Vector2Subtract(contact->end, contact->other.ball->position);
  } else {
    rOther = Vector2Zero();
  }

  contact->jacobian[0] = -n.x;
  contact->jacobian[1] = -n.y;
  contact->jacobian[2] = -CROSS(rBall, n);

  contact->jacobian[3] = n.x;
  contact->jacobian[4] = n.y;
  contact->jacobian[5] = CROSS(rOther, n);

  const float friction = get_contact_friction(contact);
  if (friction > 0.f) {
    Vector2 t = {n.y, -n.x};
    contact->jacobian[6] = -t.x;
    contact->jacobian[7] = -t.y;
    contact->jacobian[8] = -CROSS(rBall, t);
    contact->jacobian[9] = t.x;
    contact->jacobian[10] = t.y;
    contact->jacobian[11] = CROSS(rOther, t);
  }

  float C = Vector2DotProduct(Vector2Subtract(contact->end, contact->start),
                              Vector2Negate(n));
  C = MIN(0.f, C + 0.01f);
  if (dt == 0) {
    contact->bias = 0.f;
  } else {
    float e = get_contact_restitution(contact);
    Vector2 wBall = {-contact->ball->angularVelocity * rBall.y,
                     contact->ball->angularVelocity * rBall.x};
    Vector2 vBall = Vector2Add(contact->ball->velocity, wBall);

    Vector2 vOther;
    if (type == CONTACT_BALL_BALL) {
      const Vector2 wOther = {-contact->other.ball->angularVelocity * rOther.y,
                              contact->other.ball->angularVelocity * rOther.x};
      vOther = Vector2Add(contact->other.ball->velocity, wOther);
    } else {
      vOther = Vector2Zero();
    }

    contact->bias = (BETA / dt) * C +
                    e * Vector2DotProduct(Vector2Subtract(vBall, vOther), n);
  }
}

void contact_solve(Contact *contact, float dt) {

  float *j = contact->jacobian;

  float jT[6 * 2];
  mat_transpose(j, jT, 2, 6);

  float velocities[6];
  get_contact_velocities_vector(contact, velocities);

  float invM[6 * 6];
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

  float oldLambda[2];
  vec_copy(contact->cachedLambda, oldLambda, 2);

  vec_add(contact->cachedLambda, lambda, contact->cachedLambda, 2);
  contact->cachedLambda[0] = MAX(contact->cachedLambda[0], 0.f);

  const float friction = get_contact_friction(contact);
  if (friction > 0.f) {
    const float max_friction = contact->cachedLambda[0] * friction;
    contact->cachedLambda[1] = MIN(contact->cachedLambda[1], max_friction);
    contact->cachedLambda[1] = MAX(contact->cachedLambda[1], -max_friction);
  }

  vec_sub(contact->cachedLambda, oldLambda, lambda, 2);

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

float get_contact_friction(Contact *contact) {
  float secondFriction;
  if (contact->type == CONTACT_BALL_BALL)
    secondFriction = contact->other.ball->friction;
  else
    secondFriction = contact->other.wall->friction;
  return MAX(contact->ball->friction, secondFriction);
}

float get_contact_restitution(Contact *contact) {
  float secondRestitution;
  if (contact->type == CONTACT_BALL_BALL)
    secondRestitution = contact->other.ball->restitution;
  else
    secondRestitution = contact->other.wall->restitution;
  return MIN(contact->ball->restitution, secondRestitution);
}

void get_contact_velocities_vector(Contact *contact, float *velocities) {
  vec_zero(velocities, 6);

  velocities[0] = contact->ball->velocity.x;
  velocities[1] = contact->ball->velocity.y;
  velocities[2] = contact->ball->angularVelocity;

  if (contact->type == CONTACT_BALL_BALL) {
    velocities[3] = contact->other.ball->velocity.x;
    velocities[4] = contact->other.ball->velocity.y;
    velocities[5] = contact->other.ball->angularVelocity;
  }
}

void get_contact_invm_matrix(Contact *contact, float *invM) {
  mat_zero(invM, 6, 6);

  invM[mat_idx(6, 0, 0)] = contact->ball->inverseMass;
  invM[mat_idx(6, 1, 1)] = contact->ball->inverseMass;
  invM[mat_idx(6, 2, 2)] = contact->ball->inverseInertia;

  if (contact->type == CONTACT_BALL_BALL) {
    invM[mat_idx(6, 3, 3)] = contact->other.ball->inverseMass;
    invM[mat_idx(6, 4, 4)] = contact->other.ball->inverseMass;
    invM[mat_idx(6, 5, 5)] = contact->other.ball->inverseInertia;
  }
}
