#include <GL/glut.h>
#include <GL/freeglut.h>
#include <stdbool.h>
#include <stdio.h>

#include "../include/init.h"
#include "../include/globals.h"
#include "../include/structures/point.h"
#include "../include/structures/line.h"
#include "../include/structures/polygon.h"
#include "../include/structures/matrix.h"

// Inicializa o estado do OpenGL e aloca memória para as estruturas globais
void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0); // Define a cor de fundo como branco

    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, screenWidth, 0.0, screenHeight); // Configura o sistema de coordenadas 2D

    // Inicializa os ponteiros das listas encadeadas globais
    points = initialize_points();
    lines = initialize_lines();
    polygons = initialize_polygons();

    // Aloca dinamicamente o espaço para as matrizes de transformação geométrica
    TRANSLATION = allocate_matrix(3, 3);
    ROTATION = allocate_matrix(3, 3);
    SCALE = allocate_matrix(3, 3);
    SHEAR = allocate_matrix(3, 3);
    CENTROID_ROTATION = allocate_matrix(3, 3);
    CENTROID_SCALE = allocate_matrix(3, 3);
    CENTROID_SHEAR = allocate_matrix(3, 3);
    HOMOGENEOUS_POINT = allocate_matrix(3, 1); // Matriz coluna
    RESULT = allocate_matrix(3, 3);
    RESULT_P = allocate_matrix(3, 1); // Matriz coluna
}

// Libera toda a memória alocada dinamicamente antes de fechar o programa
void destruction() {
    // Esvazia as listas e libera a memória das formas desenhadas
    delete_all_points(points);
    delete_all_lines(lines);
    delete_all_polygons(polygons);

    // Libera os ponteiros das matrizes (passando 3 como o número de linhas a serem desalocadas)
    free_matrix(TRANSLATION, 3);
    free_matrix(ROTATION, 3);
    free_matrix(SCALE, 3);
    free_matrix(SHEAR, 3);
    free_matrix(CENTROID_ROTATION, 3);
    free_matrix(CENTROID_SCALE, 3);
    free_matrix(CENTROID_SHEAR, 3);
    free_matrix(HOMOGENEOUS_POINT, 3);
    free_matrix(RESULT, 3);
    free_matrix(RESULT_P, 3);
}