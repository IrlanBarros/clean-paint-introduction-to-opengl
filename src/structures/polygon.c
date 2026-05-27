#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glut.h>
#include <math.h>

#include "../../include/structures/polygon.h"
#include "../../include/transformations/animate.h"
#include "../../include/globals.h"

// Variáveis de estado global para os polígonos
static Polygons SELECTED_POLYGON = NULL;
static PolygonNode *POLYGON_UNDER_CONSTRUCTION = NULL;
static Point FIRST_POINT = {0, 0};
static Point LAST_POINT = {0, 0};
static int drawing = 0;
static float TOLERANCE = 10;
static int translating = 0;
static int rotating = 0;
static int scaling = 0;

// Inicializa a lista encadeada de polígonos
Polygons *initialize_polygons() {
    Polygons *polygons = (Polygons *) malloc(sizeof(Polygons));
    *polygons = NULL;
    return polygons;
}

// Adiciona um polígono já pronto à lista principal (função interna)
int add_polygon(Polygon polygon, Polygons *polygons) {
    if (polygons == NULL) return 0;
    PolygonNode *new_node = (PolygonNode *) malloc(sizeof(PolygonNode));
    if (new_node == NULL) return 0;
    
    new_node->polygon = polygon;
    new_node->next = *polygons;
    *polygons = new_node;
    return 1;
}

// Retorna os pontos que compõem o polígono selecionado
Points get_polygon_points() {
    if (SELECTED_POLYGON == NULL) return NULL;
    return *(SELECTED_POLYGON->polygon.points);
}

// Lógica para adicionar vértices enquanto o usuário clica na tela
int add_current_polygon_vertex(float mouseX, float mouseY, Polygons *polygons) {
    Point p = {mouseX, mouseY};
    
    // Se não está desenhando, inicializa o estado de construção
    if (!drawing) {
        POLYGON_UNDER_CONSTRUCTION = (PolygonNode *) malloc(sizeof(PolygonNode));
        if (POLYGON_UNDER_CONSTRUCTION == NULL) return 0;
        
        POLYGON_UNDER_CONSTRUCTION->polygon.points = initialize_points();
        POLYGON_UNDER_CONSTRUCTION->next = NULL;
        
        add_point(p, POLYGON_UNDER_CONSTRUCTION->polygon.points);
        LAST_POINT = p;
        FIRST_POINT = p;
        drawing = 1;
        return 1;
    }
    
    // Se clicar perto do primeiro ponto (dentro da tolerância), fecha o polígono
    if (FIRST_POINT.x >= mouseX - TOLERANCE && FIRST_POINT.x <= mouseX + TOLERANCE &&
        FIRST_POINT.y >= mouseY - TOLERANCE && FIRST_POINT.y <= mouseY + TOLERANCE) {
        finish_current_polygon(polygons);
        return 0;
    }
    
    // Caso contrário, apenas adiciona o novo vértice
    add_point(p, POLYGON_UNDER_CONSTRUCTION->polygon.points);
    LAST_POINT = p;
    return 1;
}

// Finaliza a criação inserindo o polígono na lista principal
int finish_current_polygon(Polygons *polygons) {
    if (!drawing) return 0;
    POLYGON_UNDER_CONSTRUCTION->next = *polygons;
    *polygons = POLYGON_UNDER_CONSTRUCTION;
    POLYGON_UNDER_CONSTRUCTION = NULL;
    drawing = 0;
    return 1;
}

// Interrompe a criação do polígono e libera a memória dos pontos já marcados
int cancel_current_polygon() {
    if (!drawing) return 0;
    delete_all_points(POLYGON_UNDER_CONSTRUCTION->polygon.points);
    free(POLYGON_UNDER_CONSTRUCTION);
    POLYGON_UNDER_CONSTRUCTION = NULL;
    drawing = 0;
    return 1;
}

// Renderiza a estrutura elástica (preview) enquanto o usuário desenha
int draw_polygon_preview(float mouseX, float mouseY) {
    if (!drawing) return 0;

    glColor3f(0.5, 0.5, 0.5); // Cor cinza para a prévia
    glBegin(GL_LINE_STRIP);
        convert_vertices(POLYGON_UNDER_CONSTRUCTION->polygon.points); // Reusa a função para desenhar os vértices
    glEnd();
    
    // Desenha a linha elástica do último ponto fixado até a posição atual do mouse
    glBegin(GL_LINES);
        glVertex2f(LAST_POINT.x, LAST_POINT.y);
        glVertex2f(mouseX, mouseY);
    glEnd();

    glutPostRedisplay();
    return 1;
}

// Desenha todos os polígonos finalizados
int draw_polygons(Polygons *polygons) {
    if (polygons == NULL) return 0;
    PolygonNode *aux = *polygons;
    
    while (aux != NULL) {
        if (aux == SELECTED_POLYGON) {
            glColor3f(0, 1, 0); // Cor verde para polígono selecionado
        } else if (aux == animated_polygon) {
            glColor3fv(animated_color); // Aplica a cor que está sendo gerida na animação
        } else {
            glColor3f(0, 0, 0); // Cor preta padrão para obstáculos/normais
        }

        // GL_LINE_LOOP conecta automaticamente o último ponto ao primeiro
        glBegin(GL_LINE_LOOP); 
            convert_vertices(aux->polygon.points);
        glEnd();

        // Se for o polígono animado (DVD), desenha o texto no centro
        if (aux == animated_polygon) {
            Point center = calculate_centroid(-1, *(aux->polygon.points));
            glColor3fv(animated_color);
            glRasterPos2f(center.x - 18, center.y - 6); 
            const char* msg = "DVD";
            for (const char* c = msg; *c; c++)
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }

        aux = aux->next;
    }
    return 1;
}

// Limpa o estado global do polígono selecionado
void reset_selected_polygon() {
    SELECTED_POLYGON = NULL;
}

// Usa algoritmo de contagem de interseções (Ray-Casting) para descobrir cliques internos
int select_polygon(float mouseX, float mouseY, Polygons *polygons) {
    if (polygons == NULL) return 0;
    int count = 0;
    PolygonNode *searcher = *polygons;
    
    while (searcher != NULL) {
        count = count_hit_edges(mouseX, mouseY, searcher->polygon.points);
        // Regra Ímpar-Par: Se o número de cruzamentos for ímpar, o clique foi dentro
        if (count % 2 == 1) {
            SELECTED_POLYGON = searcher;
            return 1;
        }
        searcher = searcher->next;
    }
    SELECTED_POLYGON = NULL;
    return 0;
}

// Exclui especificamente o polígono que está focado
int delete_selected_polygon(Polygons *polygons) {
    if (polygons == NULL || SELECTED_POLYGON == NULL) return 0;

    // Libera a memória da lista de pontos ANTES de liberar o polígono
    delete_all_points(SELECTED_POLYGON->polygon.points);

    if (SELECTED_POLYGON == *polygons) {
        *polygons = SELECTED_POLYGON->next;
        free(SELECTED_POLYGON);
        SELECTED_POLYGON = NULL;
        return 1;
    }

    PolygonNode *searcher = (*polygons)->next;
    PolygonNode *previous = *polygons;
    
    while (searcher != NULL && searcher != SELECTED_POLYGON) {
        previous = searcher;
        searcher = searcher->next;
    }

    if (searcher == NULL) {
        SELECTED_POLYGON = NULL;
        return 0;
    }

    previous->next = searcher->next;
    free(searcher);
    SELECTED_POLYGON = NULL;
    return 1;
}

// Varre a lista excluindo todos os polígonos
int delete_all_polygons(Polygons *polygons) {
    if (polygons == NULL) return 0;
    PolygonNode *aux = *polygons;
    
    while (aux != NULL) {
        *polygons = aux->next;
        delete_all_points(aux->polygon.points);
        free(aux);
        aux = *polygons;
    }
    free(*polygons);
    return 1;
}

/* =================== Funções Auxiliares =================== */

// Produto vetorial (p->q) x (p->r)
float cross(Point p, Point q, Point r) {
    return (q.x - p.x) * (r.y - p.y) - (q.y - p.y) * (r.x - p.x);
}

// Distância quadrada
double dist2(Point a, Point b) {
    double dx = a.x - b.x, dy = a.y - b.y;
    return dx * dx + dy * dy;
}

// Comparador para ordenar por ângulo polar (usado no Graham Scan)
static Point p0; // Ponto de referência
int cmpPolar(const void *a, const void *b) {
    Point *p1 = (Point *)a, *p2 = (Point *)b;
    float c = cross(p0, *p1, *p2);
    if (fabs(c) < 1e-9) return (dist2(p0, *p1) < dist2(p0, *p2)) ? -1 : 1;
    return (c > 0) ? -1 : 1;
}

/* =================== Graham Scan =================== */
int grahamScan(Point *points, int n, Point *hull) {
    if (n <= 1) {
        if (n == 1) hull[0] = points[0];
        return n;
    }

    // Encontrar ponto mais baixo (e mais à esquerda em caso de empate)
    int ymin = 0;
    for (int i = 1; i < n; i++) {
        if (points[i].y < points[ymin].y ||
           (points[i].y == points[ymin].y && points[i].x < points[ymin].x)) {
            ymin = i;
        }
    }
    
    // Trocar para posição 0
    Point temp = points[0];
    points[0] = points[ymin];
    points[ymin] = temp;
    p0 = points[0];

    // Ordenar por ângulo polar
    qsort(points + 1, n - 1, sizeof(Point), cmpPolar);

    // Construir fecho
    int m = 0;
    hull[m++] = points[0];
    hull[m++] = points[1];
    
    for (int i = 2; i < n; i++) {
        while (m > 1 && cross(hull[m-2], hull[m-1], points[i]) <= 0) {
            m--;
        }
        hull[m++] = points[i];
    }
    return m;
}

/* =================== Jarvis March Adaptado =================== */
// Encontra o próximo ponto no fecho entre fechos parciais
Point nextHullPoint(Point **hulls, int *sizes, int k, Point current) {
    Point best;
    int found = 0;

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < sizes[i]; j++) {
            Point candidate = hulls[i][j];

            if (!found) {
                best = candidate;
                found = 1;
            } else {
                float c = cross(current, best, candidate);
                if (c < 0 || (fabs(c) < 1e-9 && dist2(current, candidate) > dist2(current, best))) {
                    best = candidate;
                }
            }
        }
    }
    return best;
}

/* =================== Chan’s Algorithm =================== */
int chanHull(Point *points, int n, Point *hull) {
    int m = 4; // Tamanho inicial do bloco

    while (1) {
        int k = (n + m - 1) / m; // Número de blocos
        Point **partialHulls = malloc(k * sizeof(Point *));
        int *sizes = malloc(k * sizeof(int));

        // Rodar Graham Scan em cada bloco
        for (int i = 0; i < k; i++) {
            int start = i * m;
            int end = (i+1) * m;
            if (end > n) end = n;
            int size = end - start;

            partialHulls[i] = malloc(size * sizeof(Point));
            sizes[i] = grahamScan(points + start, size, partialHulls[i]);
        }

        // Jarvis March sobre os fechos parciais
        int h = 0;
        
        // Encontrar ponto inicial global (menor y, e menor x em caso de empate)
        int idx = 0;
        for (int i = 1; i < n; i++) {
            if (points[i].y < points[idx].y ||
               (points[i].y == points[idx].y && points[i].x < points[idx].x)) {
                idx = i;
            }
        }
        hull[h++] = points[idx];

        for (int step = 0; step < m; step++) {
            Point best = nextHullPoint(partialHulls, sizes, k, hull[h-1]);

            // Fecho fechado (voltamos ao início)
            if (best.x == hull[0].x && best.y == hull[0].y) {
                for (int i = 0; i < k; i++) free(partialHulls[i]);
                free(partialHulls);
                free(sizes);
                return h;
            }

            hull[h++] = best;
        }

        // Liberar memória
        for (int i = 0; i < k; i++) free(partialHulls[i]);
        free(partialHulls);
        free(sizes);

        // Aumentar m e tentar de novo
        if (m >= n) break; // Limite
        m *= m;
        if (m > n) m = n;
    }

    return 0; // Fallback, não deveria chegar aqui
}

// Função principal de conversão para fecho convexo a partir de uma lista de nós
int transform_to_convex_hull(PointNode *start_point) {
    if (start_point == NULL) return 0;

    // 1. Contar pontos
    int n = 0;
    PointNode *aux = start_point;
    while (aux != NULL) {
        n++;
        aux = aux->next;
    }

    // 2. Copiar para array
    Point *points = malloc(n * sizeof(Point));
    aux = start_point;
    for (int i = 0; i < n; i++) {
        points[i] = aux->point;
        aux = aux->next;
    }

    // 3. Rodar ChanHull
    Point *hull = malloc(n * sizeof(Point));
    int h = chanHull(points, n, hull);

    // 4. Sobrescrever lista original com o fecho
    aux = start_point;
    PointNode *prev = NULL;
    int i = 0;
    while (aux != NULL && i < h) {
        aux->point = hull[i];
        prev = aux;
        aux = aux->next;
        i++;
    }

    // 5. Cortar a lista no ponto certo
    if (prev != NULL) {
        prev->next = NULL;
    }

    // 6. Liberar nós extras
    while (aux != NULL) {
        prev = aux;
        aux = aux->next;
        free(prev);
    }

    // 7. Liberar arrays auxiliares
    free(points);
    free(hull);

    return 1;
}