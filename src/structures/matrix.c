#include <stdio.h>
#include <stdlib.h>
#include "../../include/globals.h"
#include "../../include/structures/matrix.h"
#include "../../include/structures/point.h"

// Aloca memória dinamicamente para uma matriz 2D
float** allocate_matrix(int rows, int cols) {
    float** matrix = (float**)malloc(rows * sizeof(float*));
    if (matrix == NULL) {
        return NULL;
    }
    for (int i = 0; i < rows; i++) {
        matrix[i] = (float*)malloc(cols * sizeof(float));
        if (matrix[i] == NULL) {
            // Caso falhe no meio do caminho, libera o que já foi alocado
            for(int k = 0; k < i; k++) {
                free(matrix[k]);
            }
            free(matrix);
            return NULL;
        }
    }
    return matrix;
}

// Libera a memória alocada de uma matriz 2D
void free_matrix(float** matrix, int rows) {
    if (matrix == NULL) return;
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Imprime a matriz no console (útil para debug)
void print_matrix(float** matrix, int rows, int cols) {
    if (matrix == NULL) return;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%8.2f", matrix[i][j]);
        }
        printf("\n");
    }
}

// Multiplica duas matrizes (matrizA * matrizB) e guarda em Result
void multiply_matrices(float **result,
                       float** matrixA, int rowsA, int colsA,
                       float** matrixB, int rowsB, int colsB) {

    // Regra matemática: colunas de A devem ser iguais às linhas de B
    if (colsA != rowsB) return;
    if (result == NULL) return;

    int rowsR = rowsA;
    int colsR = colsB;

    for (int i = 0; i < rowsR; i++) {
        for (int j = 0; j < colsR; j++) {
            float sum = 0.0f;
            for (int k = 0; k < colsA; k++) {
                sum += matrixA[i][k] * matrixB[k][j];
            }
            result[i][j] = sum;
        }
    }
}

// Gera a matriz identidade adaptada para Translação (tx, ty)
void translation_matrix(float **mat, float tx, float ty) {
    if (mat == NULL) {
        return;
    }
    mat[0][0] = 1; mat[0][1] = 0; mat[0][2] = tx;
    mat[1][0] = 0; mat[1][1] = 1; mat[1][2] = ty;
    mat[2][0] = 0; mat[2][1] = 0; mat[2][2] = 1;
}

// Gera a matriz identidade adaptada para Escala (sx, sy)
void scale_matrix(float **mat, float sx, float sy) {
    if (mat == NULL) {
        return;
    }
    mat[0][0] = sx; mat[0][1] = 0; mat[0][2] = 0;
    mat[1][0] = 0; mat[1][1] = sy; mat[1][2] = 0;
    mat[2][0] = 0; mat[2][1] = 0; mat[2][2] = 1;
}

// Gera a matriz identidade adaptada para Rotação (seno e cosseno)
void rotation_matrix(float **mat, float s, float c) {
    if (mat == NULL) {
        return;
    }
    mat[0][0] = c; mat[0][1] = -s; mat[0][2] = 0;
    mat[1][0] = s; mat[1][1] = c;  mat[1][2] = 0;
    mat[2][0] = 0; mat[2][1] = 0;  mat[2][2] = 1;
}

// Gera a matriz identidade adaptada para Cisalhamento
void shear_matrix(float **mat, float sh) {
    if (mat == NULL) {
        return;
    }
    mat[0][0] = 1; mat[0][1] = sh; mat[0][2] = 0;
    mat[1][0] = 0; mat[1][1] = 1;  mat[1][2] = 0;
    mat[2][0] = 0; mat[2][1] = 0;  mat[2][2] = 1;
}

// --- Transformações Compostas (Em Relação ao Centroide) ---

// Realiza a escala mantendo a posição central do objeto
void centroid_scale_matrix(float **mat, float sx, float sy, float xm, float ym) {
    if (mat == NULL) {
        return;
    }
    // 1. Move para a origem (xm, ym)
    translation_matrix(TRANSLATION, xm, ym);
    // 2. Aplica a escala
    scale_matrix(SCALE, sx, sy);
    // 3. Multiplica T * S
    multiply_matrices(RESULT, TRANSLATION, 3, 3, SCALE, 3, 3);
    // 4. Move de volta (-xm, -ym)
    translation_matrix(TRANSLATION, -xm, -ym);
    // 5. Multiplica (T*S) * T_inversa
    multiply_matrices(mat, RESULT, 3, 3, TRANSLATION, 3, 3);
}

// Realiza a rotação mantendo a posição central do objeto
void centroid_rotation_matrix(float **mat, float s, float c, float xm, float ym) {
    if (mat == NULL) {
        return;
    }
    translation_matrix(TRANSLATION, xm, ym);
    rotation_matrix(ROTATION, s, c);
    multiply_matrices(RESULT, TRANSLATION, 3, 3, ROTATION, 3, 3);
    translation_matrix(TRANSLATION, -xm, -ym);
    multiply_matrices(mat, RESULT, 3, 3, TRANSLATION, 3, 3);
}

// Realiza o cisalhamento mantendo a posição central do objeto
void centroid_shear_matrix(float **mat, float sh, float xm, float ym) {
    if (mat == NULL) {
        return;
    }
    translation_matrix(TRANSLATION, xm, ym);
    shear_matrix(SHEAR, sh);
    multiply_matrices(RESULT, TRANSLATION, 3, 3, SHEAR, 3, 3);
    translation_matrix(TRANSLATION, -xm, -ym);
    multiply_matrices(mat, RESULT, 3, 3, TRANSLATION, 3, 3);
}

// Converte um Ponto 2D cartesiano em uma matriz coluna homogênea 3x1
void homogeneous_point(float **mat, Point p) {
    if (mat == NULL) {
        return;
    }
    mat[0][0] = p.x;
    mat[1][0] = p.y;
    mat[2][0] = 1;   // Coordenada W padronizada como 1
}