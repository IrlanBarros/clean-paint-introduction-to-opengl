#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glut.h>
#include <math.h>

// Cabeçalhos criados
#include "../../include/globals.h"
#include "../../include/structures/point.h"
#include "../../include/structures/line.h"
#include "../../include/structures/matrix.h"

// Variáveis globais de estado do ponteiro e do mouse
static Points SELECTED_POINT = NULL;
static float TOLERANCE = 2;
static int translating = 0;
static int rotating = 0;
static float prevMouseX, prevMouseY;
static float curMouseX, curMouseY;

// Inicializa a lista encadeada de pontos
Points *initialize_points() {
    Points *points = (Points *) malloc(sizeof(Points));
    *points = NULL;
    return points;
}

// Adiciona um novo nó de ponto no início da lista
int add_point(Point point, Points *points) {
    if (points == NULL) return 0;
    PointNode *new_node = (PointNode *) malloc(sizeof(PointNode));
    if (new_node == NULL) return 0;
    
    new_node->point = point;
    new_node->next = *points;
    *points = new_node;
    return 1;
}

// Retorna o ponteiro para o ponto selecionado atualmente
Points get_selected_point() {
    return SELECTED_POINT;
}

// Avalia o segmento de linha formado pelo ponto inicial e seu sucessor
int line_evaluator(Points start_point, float xmin, float xmax, float ymin, float ymax) {
    Point p1 = start_point->point;
    Point p2 = start_point->next->point;
    return evaluate_line(p1, p2, xmin, xmax, ymin, ymax);
}

// Função auxiliar (helper): gera o código binário da região para o algoritmo de Cohen-Sutherland
unsigned int encode(Point p, float xmin, float xmax, float ymin, float ymax) {
    unsigned int code = 0b0000;
    if (p.x < xmin) {
        code |= 0b1000; // Esquerda
    } else if (p.x > xmax) {
        code |= 0b0100; // Direita
    }
    if (p.y < ymin) {
        code |= 0b0010; // Abaixo
    } else if (p.y > ymax) {
        code |= 0b0001; // Acima
    }
    return code;
}

// Recorta (clipping) a linha dentro da janela utilizando o algoritmo de Cohen-Sutherland
int evaluate_line(Point p1, Point p2, float xmin, float xmax, float ymin, float ymax) {
    unsigned int code1, code2, and_codes, left, right, bottom, top;
    
    code1 = encode(p1, xmin, xmax, ymin, ymax);
    code2 = encode(p2, xmin, xmax, ymin, ymax);
    and_codes = code1 & code2;
    
    left = code1 & 0b1000;
    right = code1 & 0b0100;
    bottom = code1 & 0b0010;
    top = code1 & 0b0001;
    
    if (code1 == 0b0000 || code2 == 0b0000) {
        return 1;
    } else if (and_codes == 0b0000) {
        // Encontra o ponto de interseção de acordo com a borda ultrapassada e recalcula
        if (left) {
            return evaluate_line((Point){xmin, p1.y + (xmin-p1.x)*(p2.y-p1.y)/(p2.x-p1.x)}, p2, xmin, xmax, ymin, ymax);
        } else if (right) {
            return evaluate_line((Point){xmax, p1.y + (xmax-p1.x)*(p2.y-p1.y)/(p2.x-p1.x)}, p2, xmin, xmax, ymin, ymax);
        } else if (bottom) {
            return evaluate_line((Point){p1.x + (ymin-p1.y)*(p2.x-p1.x)/(p2.y-p1.y), ymin}, p2, xmin, xmax, ymin, ymax);
        } else {
            return evaluate_line((Point){p1.x + (ymax-p1.y)*(p2.x-p1.x)/(p2.y-p1.y), ymax}, p2, xmin, xmax, ymin, ymax);
        }
    }
    return 0; // Segmento completamente fora da janela de visualização
}

// Conta o número de interseções de um raio emitido pelo mouse com as arestas do polígono (Ray-casting)
int count_hit_edges(float mouseX, float mouseY, Points *points) {
    if (points == NULL || *points == NULL) return -1;
    int count = 0;
    PointNode *aux = *points;
    Point p1, p2;
    float xi;
    
    while (aux != NULL) {
        p1 = aux->point;
        if (aux->next != NULL) {
            p2 = aux->next->point;
        } else {
            if (aux != *points) {
                p2 = (*points)->point; // Conecta o último ponto ao primeiro
            } else {
                return -1;
            }
        }
        
        // Verifica se a aresta cruza o raio horizontal originado a partir do mouse
        if (!((p1.y > mouseY && p2.y > mouseY)
                     || (p1.y < mouseY && p2.y < mouseY)
                     || (p1.x < mouseX && p2.x < mouseX))){
            if ((p1.x > mouseX && p2.x > mouseX) && ((mouseY-p1.y)*(mouseY-p2.y) < 0)) {
                count += 1;
            } else {
                if (p1.y != p2.y) {
                    xi = p1.x + (mouseY-p1.y)*(p2.x-p1.x)/(p2.y-p1.y);
                    if (xi > mouseX && xi != p2.x) {
                        count += 1;
                    }
                }
            }
        }
        aux = aux->next;
    }
    return count;
}

// Realiza as chamadas de API do OpenGL para gerar os vértices iterando a lista de pontos
int convert_vertices(Points *points) {
    if (points == NULL) return 0;
    PointNode *aux = *points;
    
    while (aux != NULL) {
        glVertex2f(aux->point.x, aux->point.y);
        aux = aux->next;
    }
    return 1;
}

// Renderiza a estrutura visualizando os pontos individualmente
int draw_points(Points *points) {
    if (points == NULL) return 0;
    PointNode *aux = *points;
    
    while (aux != NULL) {
        if (aux == SELECTED_POINT) {
            glColor3f(0, 1, 0); // O ponto selecionado fica verde
        }
        glBegin(GL_POINTS);
            glVertex2f(aux->point.x, aux->point.y);
        glEnd();
        aux = aux->next;
        glColor3f(0, 0, 0); // Retorna a cor preta padrão para os próximos desenhos
    }
    return 1;
}

// Calcula o centroide baseando-se na média das coordenadas da quantidade de pontos informados
Point calculate_centroid(int count_limit, Points start_point) {
    if (start_point == NULL) {printf("pronto"); return (Point){-1, -1};}
    float xm = 0, ym = 0;
    int count = 0;
    int aux_count = 0;
    
    if (count_limit == -1) aux_count = -2;
    PointNode *searcher = start_point;
    
    while (aux_count < count_limit && searcher != NULL) {
        xm += searcher->point.x;
        ym += searcher->point.y;
        count++;
        if (count_limit != -1) aux_count++;
        searcher = searcher->next;
    }
    
    xm /= count; ym /= count;
    return (Point){xm, ym};
}

// Aplica uma transformação matricial a uma lista de pontos utilizando coordenadas homogêneas
int transform_points(float **mat, int count_limit, Points start_point) {
    if (start_point == NULL) return 0;
    int count = 0;
    
    if (count_limit == -1) count = -2;
    PointNode *searcher = start_point;
    
    while (count < count_limit && searcher != NULL) {
        // Conversão e aplicação algébrica importada de matrizes.h
        homogeneous_point(HOMOGENEOUS_POINT, searcher->point);
        multiply_matrices(RESULT_P, mat, 3, 3, HOMOGENEOUS_POINT, 3, 1);
        searcher->point.x = RESULT_P[0][0];
        searcher->point.y = RESULT_P[1][0];
        searcher = searcher->next;
        
        if (count_limit != -1) count++;
    }

    return 1;
}

// Calcula o vetor de um determinado ponto com referência ao centroide do objeto
Point point_centroid_vector(Points point, float xm, float ym) {
    float x = point->point.x, y = point->point.y;
    return (Point){x-xm, y-ym};
}

// Limpa o estado global do ponto atualmente selecionado
void reset_selected_point() {
    SELECTED_POINT = NULL;
}

// Testa e define qual ponto da lista o usuário clicou (utiliza a distância baseada em tolerância)
int select_point(float mouseX, float mouseY, Points *points) {
    if (points == NULL) return 0;
    PointNode* searcher = *points;
    
    while (searcher != NULL) {
        if (searcher->point.x >= mouseX-TOLERANCE && searcher->point.x <= mouseX+TOLERANCE &&
            searcher->point.y >= mouseY-TOLERANCE && searcher->point.y <= mouseY+TOLERANCE) {
            SELECTED_POINT = searcher;
            return 1;
        }
        searcher = searcher->next;
    }

    SELECTED_POINT = NULL;
    return 0;
}

// Desaloca o ponto focado no momento sem quebrar o encadeamento da lista
int delete_selected_point(Points *points) {
    if (points == NULL) return 0;
    if (SELECTED_POINT == NULL) return 0;
    
    if (SELECTED_POINT == *points) {
        *points = SELECTED_POINT->next;
        free(SELECTED_POINT);
        SELECTED_POINT = NULL;
        return 1;
    }
    
    PointNode *searcher = (*points)->next;
    PointNode *previous = *points;
    
    while (searcher != NULL && searcher != SELECTED_POINT) {
        previous = searcher;
        searcher = searcher->next;
    }
    
    if (searcher == NULL) {
        SELECTED_POINT = NULL;
        return 0;
    }
    
    previous->next = searcher->next;
    free(searcher);
    SELECTED_POINT = NULL;
    return 1;
}

// Loop completo para limpar todos os nós da memória
int delete_all_points(Points *points) {
    if (points == NULL) return 0;
    PointNode *aux = *points;
    
    while (aux != NULL) {
        *points = aux->next;
        free(aux);
        aux = *points;
    }
    
    free(*points);
    return 1;
}