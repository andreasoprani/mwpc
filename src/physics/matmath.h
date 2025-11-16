#ifndef PHYSICS_MATMATH_H
#define PHYSICS_MATMATH_H

void vec_zero(float *v, const unsigned int n);
float vec_dot(const float *a, const float *b, const unsigned int n);
void vec_add(const float *a, const float *b, float *result,
             const unsigned int n);
void vec_sub(const float *a, const float *b, float *result,
             const unsigned int n);
void vec_scale(const float *a, float *result, const float scale,
               const unsigned int n);
void vec_copy(const float *a, float *result, const unsigned int n);

// Returns the index of the element at position (i, j) in a matrix of size m x
// n.
unsigned int mat_idx(const unsigned int n, const unsigned int i,
                     const unsigned int j);

float *mat_row(const float *mat, const unsigned int n, const unsigned int i);

void mat_zero(float *mat, const unsigned int m, const unsigned int n);

// Transposes a matrix of size m x n to a matrix of size n x m.
void mat_transpose(const float *mat, float *result, const unsigned int m,
                   const unsigned int n);

// Multiplies a matrix of size m x n with a vector of size n.
void mat_mul_vec(const float *mat, const float *vec, float *result,
                 const unsigned int m, const unsigned int n);

// Multiplies two matrices of size m x n and n x p.
void mat_mul_mat(const float *mat1, const float *mat2, float *result,
                 const unsigned int m, const unsigned int n,
                 const unsigned int p);

// Solves a system of linear equations using the Gauss-Seidel method.
// n is the length of the vector b.
void solve_gauss_seidel(const float *A, const float *b, float *x,
                        const unsigned int n);

#endif
