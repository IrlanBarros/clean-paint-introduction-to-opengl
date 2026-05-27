#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../../include/globals.h"
#include "../../include/structures/point.h"
#include "../../include/structures/line.h"
#include "../../include/structures/polygon.h"
#include "../../include/structures/matrix.h"
#include "../../include/transformations/transform.h"

// Função que roteia a seleção descobrindo qual objeto foi clicado
int select_object(float mouseX, float mouseY, Points *points, Lines *lines, Polygons *polygons, Points *SELECTED) {
    *SELECTED = NULL;
    
    // Limpa qualquer seleção anterior
    reset_selected_point();
    reset_selected_line();
    reset_selected_polygon();
    
    // Tenta selecionar um ponto (Retorna 1)
    if (select_point(mouseX, mouseY, points)) {
        *SELECTED = get_selected_point();
        return 1;
    }
    // Tenta selecionar uma linha (Retorna 2)
    if (select_line(mouseX, mouseY, lines)) {
        *SELECTED = get_line_points();
        return 2;
    }
    // Tenta selecionar um polígono (Retorna 3)
    if (select_polygon(mouseX, mouseY, polygons)) {
        *SELECTED = get_polygon_points();
        return 3;
    }
    
    return -1; // Nenhum objeto selecionado
}

// Roteia e aplica as transformações algébricas e geométricas baseando-se no estado da interface
int apply_transformation(int action, int object_type, float mouseX, float mouseY, int direction, Points start_point) {
    if (start_point == NULL) return 0;
    if (action < 2) return 0; // Ações 0 e 1 geralmente representam criação/edição padrão
    
    int count_limit;
    if (object_type == 1) count_limit = 1;  // Ponto: Transforma 1 vértice
    if (object_type == 2) count_limit = 2;  // Linha: Transforma 2 vértices
    if (object_type == 3) count_limit = -1; // Polígono: Transforma todos os vértices (-1 = sem limite)

    // O centroide é a âncora para a maioria das transformações
    Point centroid = calculate_centroid(count_limit, start_point);
    if (centroid.x == -1 && centroid.y == -1) return 0;
    
    float **mat;
    
    // Matriz de identidade base
    translation_matrix(TRANSLATION, 0, 0);
    mat = TRANSLATION;
    
    // --- 2: Translação ---
    if (action == 2) {
        printf("transladando\n");
        translation_matrix(TRANSLATION, mouseX - centroid.x, mouseY - centroid.y);
        mat = TRANSLATION;
    }
    
    // --- 3: Escala (via Scroll Wheel) ---
    if (action == 3) {
        printf("escalando\n");
        if (direction > 0) centroid_scale_matrix(CENTROID_SCALE, 1.1f, 1.1f, centroid.x, centroid.y);
        if (direction < 0) centroid_scale_matrix(CENTROID_SCALE, 0.9f, 0.9f, centroid.x, centroid.y);
        if (direction == 0) centroid_scale_matrix(CENTROID_SCALE, 1.0f, 1.0f, centroid.x, centroid.y);
        mat = CENTROID_SCALE;
    }
    
    // --- 4: Rotação (Drag/Mouse) ---
    if (action == 4) {
        printf("rotacionando\n");
        Point center = centroid;
        
        // Pontos rotacionam ao redor da origem, outras formas ao redor do próprio centroide
        if (object_type == 1) center = (Point){0, 0};
        
        Point vector = point_centroid_vector(start_point, center.x, center.y);
        float vx = vector.x;
        float vy = vector.y;
        
        float mousevx = mouseX - center.x;
        float mousevy = mouseY - center.y;
        
        float dot = mousevx * vx + mousevy * vy;
        float cross = -mousevx * vy + mousevy * vx;
        
        float norm_u = sqrtf(mousevx * mousevx + mousevy * mousevy);
        float norm_v = sqrtf(vx * vx + vy * vy);
        
        float cos_theta = dot / (norm_u * norm_v);
        float sin_theta = cross / (norm_u * norm_v);
        
        if (object_type == 1) {
            rotation_matrix(ROTATION, sin_theta, cos_theta);
            mat = ROTATION;
        } else {
            centroid_rotation_matrix(CENTROID_ROTATION, sin_theta, cos_theta, centroid.x, centroid.y);
            mat = CENTROID_ROTATION;
        }
    }
    
    // --- 5: Reflexão ---
    if (action == 5) {
        printf("refletindo\n");
        centroid_scale_matrix(CENTROID_SCALE, -1.0f, 1.0f, centroid.x, centroid.y);
        mat = CENTROID_SCALE;
    }
    
    // --- 6: Cisalhamento (via Scroll Wheel) ---
    if (action == 6) {
        printf("cisalhando\n");
        if (direction > 0) centroid_shear_matrix(CENTROID_SHEAR, 0.1f, centroid.x, centroid.y);
        if (direction < 0) centroid_shear_matrix(CENTROID_SHEAR, -0.1f, centroid.x, centroid.y);
        if (direction == 0) centroid_shear_matrix(CENTROID_SHEAR, 0.0f, centroid.x, centroid.y);
        mat = CENTROID_SHEAR;
    }
    
    // --- 7: Fecho Convexo (Convex Hull) ---
    if (action == 7) {
        if (object_type == 3) { // Apenas polígonos podem ser transformados em Fecho Convexo
            if (transform_to_convex_hull(start_point)) return 1;
            return 0;
        }
    }

    printf("aplicando transformacao aos pontos\n");
    
    // Aplica a matriz de transformação selecionada aos vértices do objeto
    transform_points(mat, count_limit, start_point);

    return 1;
}