#include <GL/glut.h>
#include <GL/freeglut.h>
#include <stdbool.h>
#include <stdio.h>

#include "../../include/transformations/motion.h"
#include "../../include/transformations/transform.h"
#include "../../include/globals.h"

// Captura o movimento do mouse enquanto um botão está pressionado (Drag)
void motion(int x, int y) {
    // Inverte o eixo Y para se adequar ao sistema de coordenadas 2D que definimos (gluOrtho2D)
    mouseX = (float)x;
    mouseY = (float)(screenHeight - y);

    // mode == 0 (Edição), action 2 (Translação) ou 4 (Cisalhamento) com o botão esquerdo pressionado
    if (mode == 0 && (action == 2 || action == 4) && left_button) {
        apply_transformation(action, object_type, mouseX, mouseY, 0, SELECTED);
    }

    glutPostRedisplay(); // Atualiza a tela com a nova posição
}

// Captura os eventos da rodinha do mouse (Scroll Wheel)
// Nota: O FreeGLUT usa a função glutMouseWheelFunc para passar a direção (+1 ou -1)
void wheel(int button, int dir_scroll, int x, int y) {
    direction = dir_scroll; // Salva a direção da rolagem na variável global
    
    // action 3 (Rotação) ou 6 (Escala)
    if (action == 3 || action == 6) {
        apply_transformation(action, object_type, mouseX, mouseY, direction, SELECTED);
    }
    
    glutPostRedisplay(); // Atualiza a tela com o novo tamanho/rotação
}