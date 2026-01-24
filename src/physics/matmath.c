#include "matmath.h"
#include <stdlib.h>

void vec_zero(float *v, const unsigned int n)
{
    for (unsigned int i = 0; i < n; i++) {
        v[i] = 0.0f;
    }
}

float vec_dot(const float *a, const float *b, const unsigned int n)
{
    float sum = 0.0f;
    for (unsigned int i = 0; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

void vec_add(const float *a, const float *b, float *result,
             const unsigned int n)
{
    for (unsigned int i = 0; i < n; i++) {
        result[i] = a[i] + b[i];
    }
}

void vec_sub(const float *a, const float *b, float *result,
             const unsigned int n)
{
    for (unsigned int i = 0; i < n; i++) {
        result[i] = a[i] - b[i];
    }
}

void vec_scale(const float *a, float *result, const float scale,
               const unsigned int n)
{
    for (unsigned int i = 0; i < n; i++) {
        result[i] = a[i] * scale;
    }
}

void vec_copy(const float *a, float *result, const unsigned int n)
{
    for (unsigned int i = 0; i < n; i++) {
        result[i] = a[i];
    }
}

unsigned int mat_idx(const unsigned int n, const unsigned int i,
                     const unsigned int j)
{
    return i * n + j;
}

float *mat_row(const float *mat, const unsigned int n, const unsigned int i)
{
    return (float *) mat + i * n;
}

void mat_zero(float *mat, const unsigned int m, const unsigned int n)
{
    vec_zero(mat, m * n);
}

void mat_transpose(const float *mat, float *result, const unsigned int m,
                   const unsigned int n)
{
    for (unsigned int i = 0; i < m; i++) {
        for (unsigned int j = 0; j < n; j++) {
            result[mat_idx(m, j, i)] = mat[mat_idx(n, i, j)];
        }
    }
}

void mat_mul_vec(const float *mat, const float *vec, float *result,
                 const unsigned int m, const unsigned int n)
{
    for (unsigned int i = 0; i < m; i++) {
        result[i] = vec_dot(mat_row(mat, n, i), vec, n);
    }
}

void mat_mul_mat(const float *mat1, const float *mat2, float *result,
                 const unsigned int m, const unsigned int n,
                 const unsigned int p)
{
    float mat2T[p * n]; // NOLINT(readability-identifier-naming)
    mat_transpose(mat2, mat2T, n, p);
    for (unsigned int i = 0; i < m; i++) {
        for (unsigned int j = 0; j < p; j++) {
            result[mat_idx(p, i, j)] =
                vec_dot(mat_row(mat1, n, i), mat_row(mat2T, n, j), n);
        }
    }
}

void solve_gauss_seidel(const float *A, const float *b, float *x,
                        const unsigned int n)
{
    const unsigned int iterations = n;
    for (unsigned int it = 0; it < iterations; it++) {
        for (unsigned int i = 0; i < n; i++) {
            if (A[i * n + i] != 0.f) {
                float dx =
                    (b[i] / A[mat_idx(n, i, i)]) -
                    (vec_dot(mat_row(A, n, i), x, n) / A[mat_idx(n, i, i)]);
                if (dx == dx) {
                    x[i] += dx;
                }
            }
        }
    }
}
