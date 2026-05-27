#ifndef LINE_H
#define LINE_H

#include "../../include/structures/point.h"

typedef struct {
    Points *points;
} Line;

typedef struct LineNode {
    Line line;
    struct LineNode *next;
} LineNode;

typedef LineNode* Lines;

// Inicialização e manipulação
Lines *initialize_lines();
int add_line(Line line, Lines *lines);
void reset_selected_line();
Points get_line_points();

// Criação e renderização
int create_line(float mouseX, float mouseY, Lines *lines);
int draw_line_preview(float mouseX, float mouseY);
int cancel_current_line();
int draw_lines(Lines *lines);

// Seleção
int select_line(float mouseX, float mouseY, Lines *lines);

// Exclusão
int delete_selected_line(Lines *lines);
int delete_all_lines(Lines *lines);

#endif // LINE_H