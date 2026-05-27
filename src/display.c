#include <GL/glut.h>
#include <GL/freeglut.h>
#include <stdbool.h>
#include <stdio.h>

#include "../include/display.h"
#include "../include/transformations/animate.h"
#include "../include/globals.h"

// extern bool game_over;
extern void draw_game_over();

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glPointSize(5);

    glColor3f(0.0, 0.0, 0.0); // Preto
    glBegin(GL_LINES);
    glEnd();

    draw_points(points);
    draw_lines(lines);
    draw_polygons(polygons);

    if (draw_line_preview(mouseX, mouseY));
    if (draw_polygon_preview(mouseX, mouseY));

    if (is_game_over) {
        draw_game_over();
    }

    glFlush();
}