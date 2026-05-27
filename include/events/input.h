#ifndef INPUT_H
#define INPUT_H

// Captura e processa os eventos de teclas comuns (padrão ASCII)
void keyboard(unsigned char key, int x, int y);

// Captura e processa os eventos de clique e estado dos botões do mouse
void mouse(int button, int state, int x, int y);

#endif // INPUT_H