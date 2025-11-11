#include "collision.h"
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

  contact->type = COLLISION_BALL_BALL;
  contact->ball = ball1;
  contact->other.ball = ball2;
  contact->normal = Vector2Normalize(ab);
  contact->start = Vector2Subtract(
      ball2->position, Vector2Scale(contact->normal, ball2->radius));
  contact->end =
      Vector2Add(ball1->position, Vector2Scale(contact->normal, ball1->radius));
  contact->depth = Vector2Length(Vector2Subtract(contact->end, contact->start));

  memset(&contact->jacobian, 0, sizeof contact->jacobian);
  memset(&contact->cachedLambda, 0, sizeof contact->cachedLambda);
  contact->friction = MAX(ball1->friction, ball2->friction);

  return true;
}

bool ball_wall_are_colliding(Ball *ball, Wall *wall, Contact *contact) {
  const Vector2 start_to_ball = Vector2Subtract(ball->position, wall->start);

  const Vector2 wall_closest_point = Vector2Add(
      wall->start,
      Vector2Scale(wall->direction,
                   Vector2DotProduct(start_to_ball, wall->direction)));

  const Vector2 outside_normal = wall_get_outside_normal(wall);

  const Vector2 closest_to_ball =
      Vector2Subtract(ball->position, wall_closest_point);

  const bool insideWall =
      Vector2DotProduct(closest_to_ball, outside_normal) >= 0.f;
  const bool collidingWall = !insideWall && Vector2LengthSqr(closest_to_ball) <=
                                                ball->radius * ball->radius;

  if (!insideWall && !collidingWall)
    return false;

  return true;
}

void contact_pre_solve(Contact *contact, float dt) {
  const Vector2 n = contact->normal;

  const Vector2 r1 = Vector2Subtract(contact->start, contact->ball->position);
  const Vector2 r2 =
      Vector2Subtract(contact->end, contact->other.ball->position);

  contact->jacobian[0] = -n.x;
  contact->jacobian[1] = -n.y;
  contact->jacobian[2] = -CROSS(r1, n);
  contact->jacobian[3] = n.x;
  contact->jacobian[4] = n.y;
  contact->jacobian[5] = CROSS(r2, n);

  if (contact->friction > 0.f) {
    Vector2 t = {n.y, -n.x};
    contact->jacobian[6] = -t.x;
    contact->jacobian[7] = -t.y;
    contact->jacobian[8] = -CROSS(r1, t);
    contact->jacobian[9] = t.x;
    contact->jacobian[10] = t.y;
    contact->jacobian[11] = CROSS(r2, t);
  }

  float C = Vector2DotProduct(Vector2Subtract(contact->end, contact->start),
                              Vector2Negate(n));
  C = MIN(0.f, C + 0.01f);
  if (dt == 0) {
    contact->bias = 0.f;
  } else {
    float e = MIN(contact->ball->restitution, contact->other.ball->restitution);
    Vector2 w1 = {-contact->ball->angularVelocity * r1.y,
                  contact->ball->angularVelocity * r1.x};
    Vector2 v1 = Vector2Add(contact->ball->velocity, w1);
    Vector2 w2 = {-contact->other.ball->angularVelocity * r2.y,
                  contact->other.ball->angularVelocity * r2.x};
    Vector2 v2 = Vector2Add(contact->other.ball->velocity, w2);
    contact->bias =
        (BETA / dt) * C + e * Vector2DotProduct(Vector2Subtract(v1, v2), n);
  }
}

void contact_solve(Contact *contact, float dt) {
  float *j = contact->jacobian;
  float jT[6 * 2];
  mat_transpose(contact->jacobian, jT, 2, 6);

  float velocities[6] = {
      contact->ball->velocity.x,       contact->ball->velocity.y,
      contact->ball->angularVelocity,  contact->other.ball->velocity.x,
      contact->other.ball->velocity.y, contact->other.ball->angularVelocity,
  };

  float invM[6 * 6] = {
      contact->ball->inverseMass,          0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
      contact->ball->inverseMass,          0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
      contact->ball->inverseInertia,       0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
      contact->other.ball->inverseMass,    0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
      contact->other.ball->inverseMass,    0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
      contact->other.ball->inverseInertia,
  };

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

  if (contact->friction > 0.f) {
    const float max_friction = contact->cachedLambda[0] * contact->friction;
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
  ball_apply_impulse_linear(contact->other.ball, il2);
  ball_apply_impulse_angular(contact->other.ball, ia2);
}
