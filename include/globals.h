#ifndef GLOBALS_H
#define GLOBALS_H
#include <stdbool.h>

#include "../include/structures/point.h"
#include "../include/structures/line.h"
#include "../include/structures/polygon.h"

// --- Matrizes de Transformação Padrão ---
extern float **TRANSLATION;
extern float **ROTATION;
extern float **SCALE;
extern float **SHEAR;

// --- Matrizes de Transformação em relação ao Centroide ---
extern float **CENTROID_ROTATION;
extern float **CENTROID_SCALE;
extern float **CENTROID_SHEAR;

// --- Matrizes Auxiliares e de Resultado ---
extern float **HOMOGENEOUS_POINT;
extern float **RESULT;
extern float **RESULT_P;

// --- Estado da Animação ---
extern float animated_color[3];
extern Polygons animated_polygon;
extern bool is_game_over;

// --- Estado Global de Interface e Interação ---
extern int object_type;     // Corresponde à variável 'objeto'
extern int mode;            // Corresponde à variável 'modo'
extern int action;          // Ação atual (ex: transladar, rotacionar)
extern int direction;       // Corresponde à variável 'dir'
extern Points SELECTED;     // Ponto atualmente selecionado

// --- Listas Globais de Estruturas ---
extern Points *points;
extern Lines *lines;
extern Polygons *polygons;

// --- Controle de Mouse e Dimensões da Tela ---
extern float mouseX, mouseY;
extern int left_button;     // Corresponde ao clique 'esquerdo'
extern int right_button;    // Corresponde ao clique 'direito'
extern int screenWidth, screenHeight;

#endif // GLOBALS_H