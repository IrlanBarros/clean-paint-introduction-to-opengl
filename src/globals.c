#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/globals.h"
#include "../include/structures/point.h"
#include "../include/structures/line.h"
#include "../include/structures/polygon.h"

// --- Matrizes de Transformação Padrão ---
float **TRANSLATION = NULL;
float **ROTATION = NULL;
float **SCALE = NULL;
float **SHEAR = NULL;

// --- Matrizes de Transformação em relação ao Centroide ---
float **CENTROID_ROTATION = NULL;
float **CENTROID_SCALE = NULL;
float **CENTROID_SHEAR = NULL;

// --- Matrizes Auxiliares e de Resultado ---
float **HOMOGENEOUS_POINT = NULL;
float **RESULT = NULL;
float **RESULT_P = NULL;

// --- Estado da Animação ---
float animated_color[3] = {1.0f, 0.0f, 0.0f};
Polygons animated_polygon = NULL;
bool is_game_over = false;

// --- Estado Global de Interface e Interação ---
int object_type = -1;
int mode = -1;
int action = -1;
int direction = 0;
Points SELECTED = NULL;

// --- Listas Globais de Estruturas ---
Points *points = NULL;
Lines *lines = NULL;
Polygons *polygons = NULL;

// --- Controle de Mouse e Dimensões da Tela ---
float mouseX = 0;
float mouseY = 0;
int left_button = 0;
int right_button = 0;

int screenWidth = 1280;
int screenHeight = 720;