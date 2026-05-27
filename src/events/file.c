#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/events/file.h"
#include "../../include/structures/point.h"
#include "../../include/structures/line.h"
#include "../../include/structures/polygon.h"

// Função auxiliar interna para adicionar pontos no final da lista durante o carregamento
// Isso previne que a ordem dos vértices seja invertida ao ler do arquivo
static void append_point(Point p, Points *points) {
    PointNode *new_node = (PointNode *)malloc(sizeof(PointNode));
    if (!new_node) return;
    
    new_node->point = p;
    new_node->next = NULL;
    
    if (*points == NULL) {
        *points = new_node;
    } else {
        PointNode *aux = *points;
        while (aux->next != NULL) {
            aux = aux->next;
        }
        aux->next = new_node;
    }
}

// Salva o estado atual de todas as listas geométricas no disco
int save_file(const char *filepath, Points *points, Lines *lines, Polygons *polygons) {
    FILE *file = fopen(filepath, "w");
    if (file == NULL) return 0;

    // 1. Salvar Pontos
    PointNode *p_aux = (points != NULL) ? *points : NULL;
    while (p_aux != NULL) {
        fprintf(file, "POINT %f %f\n", p_aux->point.x, p_aux->point.y);
        p_aux = p_aux->next;
    }

    // 2. Salvar Linhas
    LineNode *l_aux = (lines != NULL) ? *lines : NULL;
    while (l_aux != NULL) {
        // Conta quantos pontos existem na linha
        int count = 0;
        PointNode *lp_aux = *(l_aux->line.points);
        while (lp_aux != NULL) { count++; lp_aux = lp_aux->next; }
        
        fprintf(file, "LINE %d\n", count);
        lp_aux = *(l_aux->line.points);
        while (lp_aux != NULL) {
            fprintf(file, "%f %f\n", lp_aux->point.x, lp_aux->point.y);
            lp_aux = lp_aux->next;
        }
        l_aux = l_aux->next;
    }

    // 3. Salvar Polígonos
    PolygonNode *poly_aux = (polygons != NULL) ? *polygons : NULL;
    while (poly_aux != NULL) {
        int count = 0;
        PointNode *pp_aux = *(poly_aux->polygon.points);
        while (pp_aux != NULL) { count++; pp_aux = pp_aux->next; }
        
        fprintf(file, "POLYGON %d\n", count);
        pp_aux = *(poly_aux->polygon.points);
        while (pp_aux != NULL) {
            fprintf(file, "%f %f\n", pp_aux->point.x, pp_aux->point.y);
            pp_aux = pp_aux->next;
        }
        poly_aux = poly_aux->next;
    }

    fclose(file);
    return 1;
}

// Carrega os dados de um arquivo texto e reconstrói as listas encadeadas
int load_file(const char *filepath, Points *points, Lines *lines, Polygons *polygons) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) return 0;

    // Limpa as estruturas atuais antes de carregar
    delete_all_points(points);
    delete_all_lines(lines);
    delete_all_polygons(polygons);

    char type[20];
    // Lê o tipo de objeto linha por linha
    while (fscanf(file, "%19s", type) != EOF) {
        
        if (strcmp(type, "POINT") == 0) {
            float x, y;
            fscanf(file, "%f %f", &x, &y);
            // Pontos isolados podem ser adicionados no início normalmente
            add_point((Point){x, y}, points);
            
        } else if (strcmp(type, "LINE") == 0) {
            int count;
            fscanf(file, "%d", &count);
            Points *new_points = initialize_points();
            
            for (int i = 0; i < count; i++) {
                float x, y;
                fscanf(file, "%f %f", &x, &y);
                append_point((Point){x, y}, new_points);
            }
            add_line((Line){new_points}, lines);
            
        } else if (strcmp(type, "POLYGON") == 0) {
            int count;
            fscanf(file, "%d", &count);
            Points *new_points = initialize_points();
            
            for (int i = 0; i < count; i++) {
                float x, y;
                fscanf(file, "%f %f", &x, &y);
                append_point((Point){x, y}, new_points);
            }
            add_polygon((Polygon){new_points}, polygons);
        }
    }

    fclose(file);
    return 1;
}