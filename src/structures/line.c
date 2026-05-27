#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glut.h>
#include <math.h>
#include "../../include/globals.h"
#include "../../include/structures/point.h"
#include "../../include/structures/line.h"
#include "../../include/structures/matrix.h"

// Variáveis de estado global para as linhas
static Lines SELECTED_LINE = NULL;
static Point PIVOT = {0, 0};
static float TOLERANCE = 10;
static int drawing = 0;
static int translating = 0;
static int rotating = 0;
static int scaling = 0;

// Inicializa a lista encadeada de linhas
Lines *initialize_lines() {
    Lines *lines = (Lines *) malloc(sizeof(Lines));
    *lines = NULL;
    return lines;
}

// Adiciona um novo nó de linha no início da lista
int add_line(Line line, Lines *lines) {
    if (lines == NULL) return 0;
    LineNode *new_node = (LineNode *) malloc(sizeof(LineNode));
    if (new_node == NULL) return 0;
    
    new_node->line = line;
    new_node->next = *lines;
    *lines = new_node;
    return 1;
}

// Limpa o estado global da linha atualmente selecionada
void reset_selected_line() {
    SELECTED_LINE = NULL;
}

// Retorna os pontos que compõem a linha selecionada
Points get_line_points() {
    if (SELECTED_LINE == NULL) return NULL;
    return *(SELECTED_LINE->line.points);
}

// Lógica de criação de linha dividida em dois cliques (início e fim)
int create_line(float mouseX, float mouseY, Lines *lines) {
    // Primeiro clique: define o pivô inicial e ativa o modo de desenho
    if (!drawing) {
        PIVOT = (Point){mouseX, mouseY};
        drawing = 1;
        return 1;
    }
    
    // Segundo clique: finaliza a linha, salva os dois pontos e adiciona à lista
    drawing = 0;
    Points *points = initialize_points();
    add_point(PIVOT, points);
    add_point((Point){mouseX, mouseY}, points);
    add_line((Line){points}, lines);
    return 1;
}

// Desenha a "linha elástica" que acompanha o mouse antes do segundo clique
int draw_line_preview(float mouseX, float mouseY) {
    if (!drawing) return 0;
    glBegin(GL_LINES);
        glVertex2f(PIVOT.x, PIVOT.y);
        glVertex2f(mouseX, mouseY);
    glEnd();
    glutPostRedisplay();
    return 1;
}

// Interrompe a criação de uma linha no meio do processo
int cancel_current_line() {
    if (!drawing) return 0;
    drawing = 0;
    return 1;
}

// Renderiza todas as linhas da lista
int draw_lines(Lines *lines) {
    if (lines == NULL) return 0;
    LineNode *aux = *lines;
    
    while (aux != NULL) {
        if (aux == SELECTED_LINE) {
            glColor3f(0, 1, 0); // Cor verde para a linha selecionada
        }
        glBegin(GL_LINES);
            convert_vertices(aux->line.points); // Chama a função do point.c para renderizar os vértices
        glEnd();
        aux = aux->next;
        glColor3f(0, 0, 0); // Reseta para a cor preta padrão
    }
    return 1;
}

// Testa e define qual linha da lista o usuário clicou (utiliza bounding box com tolerância)
int select_line(float mouseX, float mouseY, Lines *lines) {
    if (lines == NULL) return 0;
    float xmin = mouseX - TOLERANCE, ymin = mouseY - TOLERANCE, 
          xmax = mouseX + TOLERANCE, ymax = mouseY + TOLERANCE;
          
    LineNode* searcher = *lines;
    
    while (searcher != NULL) {
        // Envia os pontos da linha para o avaliador (Cohen-Sutherland em point.c)
        if(line_evaluator(*(searcher->line.points), xmin, xmax, ymin, ymax)) {
            SELECTED_LINE = searcher;
            return 1;
        }
        searcher = searcher->next;
    }
    
    SELECTED_LINE = NULL;
    return 0;
}

// Função para excluir a linha atualmente em SELECTED_LINE
int delete_selected_line(Lines *lines) {
    if (lines == NULL) return 0;
    if (SELECTED_LINE == NULL) return 0;

    // Desaloca os pontos associados à linha primeiro para evitar memory leak
    delete_all_points(SELECTED_LINE->line.points);

    // Caso 1: A linha selecionada é o primeiro elemento da lista
    if (SELECTED_LINE == *lines) {
        *lines = SELECTED_LINE->next;
        free(SELECTED_LINE);
        SELECTED_LINE = NULL;
        return 1;
    }

    // Caso 2: A linha selecionada está no meio ou no fim da lista
    LineNode *searcher = (*lines)->next;
    LineNode *previous = *lines;
    
    while (searcher != NULL && searcher != SELECTED_LINE) {
        previous = searcher;
        searcher = searcher->next;
    }

    // Se não encontrou a linha na lista (salvaguarda)
    if (searcher == NULL) {
        SELECTED_LINE = NULL;
        return 0;
    }

    // Remove o elemento da lista
    previous->next = searcher->next;
    free(searcher);
    SELECTED_LINE = NULL;
    return 1;
}

// Loop completo para limpar todas as linhas e seus respectivos pontos da memória
int delete_all_lines(Lines *lines) {
    if (lines == NULL) return 0;
    LineNode *aux = *lines;
    
    while (aux != NULL) {
        delete_all_points(aux->line.points); // Desaloca os pontos contidos na linha
        *lines = aux->next;
        free(aux);
        aux = *lines;
    }
    
    free(*lines);
    return 1;
}