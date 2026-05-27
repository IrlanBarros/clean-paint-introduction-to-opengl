#ifndef POINTS_H
#define POINTS_H

typedef struct {
    float x;
    float y;
} Point;

typedef struct PointNode {
    Point point;
    struct PointNode *next;
} PointNode;

typedef struct PointNode *Points;

// Inicialização e manipulação
Points *initialize_points();
int add_point(Point point, Points *points);
Points get_selected_point();

// Utilidades geométricas
int line_evaluator(Points start_point, float xmin, float xmax, float ymin, float ymax);
int evaluate_line(Point p1, Point p2, float xmin, float xmax, float ymin, float ymax);
int count_hit_edges(float mouseX, float mouseY, Points *points);

// Renderização
int convert_vertices(Points *points);
int draw_points(Points *points);

// Transformações
Point calculate_centroid(int count, Points start_point);
int transform_points(float **mat, int count, Points start_point);
Point point_centroid_vector(Points point, float xm, float ym);

// Seleção e reset
void reset_selected_point();
int select_point(float mouseX, float mouseY, Points *points);

// Exclusão
int delete_selected_point(Points *points);
int delete_all_points(Points *points);

#endif // POINTS_H