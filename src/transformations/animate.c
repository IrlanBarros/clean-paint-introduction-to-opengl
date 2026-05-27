#include <GL/glut.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h> 
#include <math.h>

#include "../../include/globals.h"
#include "../../include/structures/line.h"
#include "../../include/structures/polygon.h"
#include "../../include/structures/point.h"
#include "../../include/structures/matrix.h"
#include "../../include/transformations/animate.h"

// Controle da animação (movimentação e estado)
static float dvd_dx = 4, dvd_dy = 4;
static bool animating = false;

// Converte o sistema de cores HSV para RGB
void hsv_to_rgb(float h, float s, float v, float rgb[3]) {
    float c = v * s;
    float x = c * (1 - fabs(fmod(h * 6, 2) - 1));
    float m = v - c;
    float r, g, b;

    if      (h < 1.0/6) { r = c; g = x; b = 0; }
    else if (h < 2.0/6) { r = x; g = c; b = 0; }
    else if (h < 3.0/6) { r = 0; g = c; b = x; }
    else if (h < 4.0/6) { r = 0; g = x; b = c; }
    else if (h < 5.0/6) { r = x; g = 0; b = c; }
    else                { r = c; g = 0; b = x; }

    rgb[0] = r + m;
    rgb[1] = g + m;
    rgb[2] = b + m;
}

// Gera uma cor aleatória
void randomize_color(float color[3]) {
    float h, s, v, new_color[3];
    do {
        h = (float)rand() / RAND_MAX;                 // Matiz: 0 a 1
        s = 0.7f + 0.3f * ((float)rand() / RAND_MAX); // Saturação: 0.7 a 1.0 (cores vivas)
        v = 0.7f + 0.3f * ((float)rand() / RAND_MAX); // Valor: 0.7 a 1.0 (brilho alto)
        hsv_to_rgb(h, s, v, new_color);
    // Garante que a nova cor não seja parecida com a atual
    } while (
        (fabs(new_color[0] - color[0]) < 0.15f && 
         fabs(new_color[1] - color[1]) < 0.15f && 
         fabs(new_color[2] - color[2]) < 0.15f)
    );
    color[0] = new_color[0];
    color[1] = new_color[1];
    color[2] = new_color[2];
}

// Reinicia todas as variáveis e estados da animação
void reset_animation() {
    dvd_dx = 4;
    dvd_dy = 4;
    animating = false;
    is_game_over = false;
    animated_polygon = NULL;
    glutPostRedisplay();
}

// Exclui todos os elementos renderizados na tela
void delete_all() {
    if (points) delete_all_points(points);
    if (lines) delete_all_lines(lines);
    if (polygons) delete_all_polygons(polygons);
}

// Renderiza a tela de fim de jogo
void draw_game_over() {
    glColor3f(1, 0, 0); // Texto vermelho
    glRasterPos2f(screenWidth/2 - 60, screenHeight/2);
    const char* msg = "GAME OVER";
    for (const char* c = msg; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

// Função auxiliar genérica para verificar interseção entre duas áreas
bool bbox_collision(float minA, float maxA, float minB, float maxB) {
    return (minA <= maxB && maxA >= minB);
}

// Calcula a caixa delimitadora (Bounding Box) de um conjunto de pontos
void calculate_bounding_box(Points pts, float *minX, float *maxX, float *minY, float *maxY) {
    *minX = *minY = 1e9;
    *maxX = *maxY = -1e9;
    for (Points v = pts; v != NULL; v = v->next) {
        if (v->point.x < *minX) *minX = v->point.x;
        if (v->point.x > *maxX) *maxX = v->point.x;
        if (v->point.y < *minY) *minY = v->point.y;
        if (v->point.y > *maxY) *maxY = v->point.y;
    }
}

// Verifica se houve colisão entre um polígono e uma Bounding Box específica
bool check_polygon_collision(Polygons p, float minX, float maxX, float minY, float maxY) {
    for (Polygons q = (polygons ? *polygons : NULL); q != NULL; q = q->next) {
        if (q == p) continue; // Não colide consigo mesmo
        
        float qminX, qmaxX, qminY, qmaxY;
        calculate_bounding_box(*(q->polygon.points), &qminX, &qmaxX, &qminY, &qmaxY);
        
        if (bbox_collision(minX, maxX, qminX, qmaxX) && bbox_collision(minY, maxY, qminY, qmaxY)) {
            return true;
        }
    }
    return false;
}

// Lógica para o polígono "comer" (absorver) um ponto
bool eat_point(float minX, float maxX, float minY, float maxY, Polygons p) {
    if (points && *points) {
        Points *pptr = points;
        Points prev = NULL, curr = *pptr;
        
        while (curr != NULL) {
            float px = curr->point.x, py = curr->point.y;
            if (px >= minX && px <= maxX && py >= minY && py <= maxY) {
                // Atualiza a lista removendo o ponto engolido
                if (prev == NULL) {
                    *pptr = curr->next;
                } else {
                    prev->next = curr->next;
                }
                Points to_free = curr;
                curr = curr->next;
                free(to_free);
                
                // Escala em 10% (cresce) em torno do centroide
                Point center = calculate_centroid(-1, *(p->polygon.points));
                float **mat = allocate_matrix(3, 3);
                centroid_scale_matrix(mat, 1.1f, 1.1f, center.x, center.y);
                transform_points(mat, -1, *(p->polygon.points));
                free_matrix(mat, 3);
                return true;
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
    }
    return false;
}

// Lógica para o polígono "comer" (absorver) uma linha
bool eat_line(float minX, float maxX, float minY, float maxY, Polygons p) {
    if (lines && *lines) {
        Lines *lptr = lines;
        Lines prevL = NULL, currL = *lptr;
        
        while (currL != NULL) {
            float lminX, lmaxX, lminY, lmaxY;
            calculate_bounding_box(*(currL->line.points), &lminX, &lmaxX, &lminY, &lmaxY);
            
            if (bbox_collision(minX, maxX, lminX, lmaxX) && bbox_collision(minY, maxY, lminY, lmaxY)) {
                // Atualiza a lista removendo a linha engolida
                if (prevL == NULL) {
                    *lptr = currL->next;
                } else {
                    prevL->next = currL->next;
                }
                delete_all_points(currL->line.points);
                Lines to_free = currL;
                currL = currL->next;
                free(to_free);
                
                // Diminui em 10% (encolhe) em torno do centroide
                Point center = calculate_centroid(-1, *(p->polygon.points));
                float **mat = allocate_matrix(3, 3);
                centroid_scale_matrix(mat, 0.9f, 0.9f, center.x, center.y);
                transform_points(mat, -1, *(p->polygon.points));
                free_matrix(mat, 3);
                return true;
            } else {
                prevL = currL;
                currL = currL->next;
            }
        }
    }
    return false;
}

// Função de callback principal para atualizar os quadros (frames)
void animate(int value) {
    if (!animating || is_game_over) return;

    Polygons p = animated_polygon;
    if (p) {
        float minX, maxX, minY, maxY;
        calculate_bounding_box(*(p->polygon.points), &minX, &maxX, &minY, &maxY);

        // Colisão com as bordas da janela
        bool changed = false;
        if (minX + dvd_dx < 0 || maxX + dvd_dx > screenWidth) {
            dvd_dx = -dvd_dx;
            changed = true;
        }
        if (minY + dvd_dy < 0 || maxY + dvd_dy > screenHeight) {
            dvd_dy = -dvd_dy;
            changed = true;
        }
        if (changed) {
            randomize_color(animated_color);
        }

        // Colisão com outros polígonos (Condição de Game Over)
        if (check_polygon_collision(animated_polygon, minX, maxX, minY, maxY)) {
            animating = false;
            is_game_over = true;
            delete_all();
            glutPostRedisplay();
            return;
        }

        // Interação com as formas (come e cresce / come e encolhe)
        eat_point(minX, maxX, minY, maxY, p);
        eat_line(minX, maxX, minY, maxY, p);

        // Verifica se o polígono encolheu demais até sumir (Condição de Game Over)
        calculate_bounding_box(*(p->polygon.points), &minX, &maxX, &minY, &maxY);
        float width = maxX - minX;
        float height  = maxY - minY;
        if (width < 1.0f || height < 1.0f) {
            animating = false;
            is_game_over = true;
            delete_all();
            glutPostRedisplay();
            return;
        }

        // Aplica matriz de translação progressiva
        float mat[3][3] = {
            {1, 0, dvd_dx},
            {0, 1, dvd_dy},
            {0, 0, 1}
        };
        float *mat_ptr[3] = {mat[0], mat[1], mat[2]}; // Array de ponteiros pra simular float**
        transform_points(mat_ptr, -1, *(p->polygon.points));
    }

    glutPostRedisplay();
    glutTimerFunc(33, animate, 0); // Repete a chamada gerando um framerate (aprox 30 FPS)
}

// Inicia o ciclo de animação
void start_animation() {
    if (!animating) {
        // Se o polígono não foi pré-definido, pega o primeiro da lista global
        if (animated_polygon == NULL && polygons && *polygons) {
            animated_polygon = *polygons;
        }
        animating = true;
        glutTimerFunc(33, animate, 0);
    }
}

// Interrompe o ciclo de animação
void stop_animation() {
    animating = false;
}