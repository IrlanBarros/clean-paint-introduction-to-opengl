#ifndef MATRIX_H
#define MATRIX_H

#include "../../include/structures/point.h"

// Alocação e manipulação de memória
float** allocate_matrix(int rows, int cols);
void free_matrix(float** matrix, int rows);
void print_matrix(float** matrix, int rows, int cols);

// Operações matemáticas
void multiply_matrices(float **result,
                       float** matrixA, int rowsA, int colsA,
                       float** matrixB, int rowsB, int colsB);

// Transformações geométricas básicas (Matrizes 3x3)
void translation_matrix(float **mat, float tx, float ty);
void scale_matrix(float **mat, float sx, float sy);
void rotation_matrix(float **mat, float s, float c);
void shear_matrix(float **mat, float sh); // Cisalhamento

// Transformações geométricas em relação ao centroide
void centroid_scale_matrix(float **mat, float sx, float sy, float xm, float ym);
void centroid_rotation_matrix(float **mat, float s, float c, float xm, float ym);
void centroid_shear_matrix(float **mat, float sh, float xm, float ym);

// Conversão de coordenadas
void homogeneous_point(float **mat, Point p);

#endif // MATRIX_H