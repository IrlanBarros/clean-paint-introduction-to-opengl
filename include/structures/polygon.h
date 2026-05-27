#ifndef POLYGON_H
#define POLYGON_H

#include "../../include/structures/point.h"

typedef struct {
    Points *points;
} Polygon;

typedef struct PolygonNode {
    Polygon polygon;
    struct PolygonNode *next;
} PolygonNode;

typedef struct PolygonNode *Polygons;

// Inicialização e manipulação
Polygons *initialize_polygons();
int add_polygon(Polygon polygon, Polygons *polygons);
Points get_polygon_points();

// Criação e edição
int add_current_polygon_vertex(float mouseX, float mouseY, Polygons *polygons);
int finish_current_polygon(Polygons *polygons);
int cancel_current_polygon();
int draw_polygon_preview(float mouseX, float mouseY);

// Renderização
int draw_polygons(Polygons *polygons);

// Seleção e reset
void reset_selected_polygon();
int select_polygon(float mouseX, float mouseY, Polygons *polygons);

// Exclusão
int delete_selected_polygon(Polygons *polygons);
int delete_all_polygons(Polygons *polygons);

// Algoritmos avançados
int transform_to_convex_hull(PointNode *start_point);

#endif // POLYGON_H