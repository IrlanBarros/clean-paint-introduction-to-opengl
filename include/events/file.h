#ifndef FILE_H
#define FILE_H

#include "../../include/structures/point.h"
#include "../../include/structures/line.h"
#include "../../include/structures/polygon.h"

// Salva os objetos geométricos em um arquivo txt
int save_file(const char *filepath, Points *points, Lines *lines, Polygons *polygons);

// Carrega os objetos geométricos de um arquivo txt
int load_file(const char *filepath, Points *points, Lines *lines, Polygons *polygons);

#endif // FILE_H