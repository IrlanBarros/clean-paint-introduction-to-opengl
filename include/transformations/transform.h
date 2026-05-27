#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../../include/structures/point.h"
#include "../../include/structures/line.h"
#include "../../include/structures/polygon.h"

// Verifica se há algum objeto (ponto, linha ou polígono) próximo às coordenadas do mouse e o seleciona
int select_object(float mouseX, float mouseY, Points *points, Lines *lines, Polygons *polygons, Points *SELECTED);

// Aplica a transformação geométrica desejada (translação, rotação, escala, cisalhamento) ao objeto
int apply_transformation(int action, int object_type, float mouseX, float mouseY, int direction, Points start_point);

#endif // TRANSFORM_H