#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include "../../include/events/input.h"
#include "../../include/globals.h"
#include "../../include/events/file.h"
#include "../../include/structures/point.h"
#include "../../include/structures/line.h"
#include "../../include/structures/polygon.h"
#include "../../include/transformations/transform.h"
#include "../../include/transformations/animate.h"
#include "../../include/transformations/motion.h"

// Gerencia eventos de teclado comuns (Padrão ASCII)
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        // --- Seleção de Objeto ---
        case '1':
            object_type = 1; // Ponto
            break;
        case '2':
            object_type = 2; // Linha
            break;
        case '3':
            object_type = 3; // Polígono
            break;
            
        // --- Modos Globais ---
        case 's':
            mode = 0; // Modo Selecionar
            object_type = -1;
            action = -1;
            break;
        case 'd':
            mode = 1; // Modo Desenhar
            action = -1;
            break;
            
        // --- Exclusão ---
        case 'x':
            if (delete_selected_point(points));
            if (delete_selected_line(lines));
            if (delete_selected_polygon(polygons));
            glutPostRedisplay();
            break;
            
        // --- Transformações Geométricas ---
        case 't':
            action = 2; // Transladar (Mouse Drag)
            break;
        case 'e':
            action = 3; // Escalar (Scroll)
            break;
        case 'r':
            action = 4; // Rotacionar (Mouse Drag)
            break;
        case 'i':
            action = 5; // Refletir
            // Reflexão aplica instantaneamente
            apply_transformation(action, object_type, mouseX, mouseY, 0, SELECTED);
            glutPostRedisplay();
            break;
        case 'c':
            action = 6; // Cisalhar (Scroll)
            break;
        case 'f':
            action = 7; // Fecho Convexo
            // Fecho convexo aplica instantaneamente
            apply_transformation(action, object_type, mouseX, mouseY, 0, SELECTED);
            glutPostRedisplay();
            break;

        // --- Arquivos (Save / Load) ---
        case 'u':
            save_file("projeto_grafico.txt", points, lines, polygons);
            printf("Objetos salvos em projeto_grafico.txt\n");
            break;
        case 'k':
            load_file("projeto_grafico.txt", points, lines, polygons);
            glutPostRedisplay();
            printf("Objetos carregados de projeto_grafico.txt\n");
            break;

        // --- Animação ---
        case 'a':
            start_animation();
            break;
        case 'b':
            stop_animation();
            break;
        case 'z':
            reset_animation();
            break;

        // --- Resets Diversos ---
        case 'p':
            mode = -1;
            object_type = -1;
            action = -1;
            break;
            
        case 27: // Tecla ESC
            exit(0);
            break;
            
        default:
            object_type = -1; // Nenhum
            mode = -1;        // Nenhum
            action = -1;
            break;
    }
}

// Gerencia cliques e estado dos botões do mouse
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        left_button = 1;
        direction = 0;
        
        // Inverte o eixo Y (GLUT vs gluOrtho2D)
        mouseX = (float)x;
        mouseY = (float)(screenHeight - y);
        
        // Se estiver no Modo de Desenho
        if (mode == 1) {
            if (object_type == 1) {
                add_point((Point){mouseX, mouseY}, points);
            } else if (object_type == 2) {
                create_line(mouseX, mouseY, lines);
            } else if (object_type == 3) {
                add_current_polygon_vertex(mouseX, mouseY, polygons);
            }
        } 
        // Se estiver no Modo de Seleção
        else if (mode == 0) {
            // Retorna o tipo de objeto que foi clicado e salva a referência em SELECTED
            object_type = select_object(mouseX, mouseY, points, lines, polygons, &SELECTED);
        }
        
        glutPostRedisplay();
    }
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        left_button = 0;
    }
    
    // Botão Direito para cancelar construções ativas
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (mode == 1) {
            if (object_type == 3) {
                cancel_current_polygon();
            } else if (object_type == 2) {
                cancel_current_line();
            }
            glutPostRedisplay();
        }
    }
}