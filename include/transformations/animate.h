#ifndef ANIMATE_H
#define ANIMATE_H

#include <stdbool.h>
#include "../../include/structures/point.h"
#include "../../include/structures/line.h"
#include "../../include/structures/polygon.h"

// --- Gerenciamento de Cores ---
// Converte o sistema de cores HSV para RGB
void hsv_to_rgb(float h, float s, float v, float rgb[3]);
// Gera uma cor aleatória
void randomize_color(float color[3]);

// --- Controle de Estado Global ---
// Reinicia todas as variáveis e estados da animação
void reset_animation();
// Exclui todos os elementos renderizados na tela
void delete_all();

// --- Lógica de Colisão e Interação (Bounding Box) ---
// Calcula a caixa delimitadora (Bounding Box) de um conjunto de pontos
void calculate_bounding_box
    (
        Points pts, 
        float *minX, 
        float *maxX, 
        float *minY, 
        float *maxY
    );

// Verifica se houve colisão entre um polígono e uma Bounding Box específica
bool check_polygon_collision
    (
        Polygons p, 
        float minX, 
        float maxX, 
        float minY, 
        float maxY
    );

// Lógica para o polígono "comer" (absorver) um ponto
bool eat_point
    (
        float minX, 
        float maxX, 
        float minY, 
        float maxY, 
        Polygons p
    );

// Lógica para o polígono "comer" (absorver) uma linha
bool eat_line
    (
        float minX, 
        float maxX, 
        float minY, 
        float maxY, 
        Polygons p
    );

// Função auxiliar genérica para verificar interseção entre duas áreas delimitadoras em um eixo
bool bbox_collision(float minA, float maxA, float minB, float maxB);

// --- Telas e Loop de Animação ---
// Renderiza a tela de fim de jogo
void draw_game_over();

// Inicia o ciclo de animação
void start_animation();
// Interrompe o ciclo de animação
void stop_animation();
// Função de callback principal para atualizar os quadros (frames)
void animate(int value);

#endif // ANIMATE_H