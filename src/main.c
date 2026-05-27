#include <GL/glut.h>
#include <GL/freeglut.h>
#include <stdbool.h>
#include <stdio.h>
//#include <windows.h>
#include <time.h> 
#include "../include/init.h"
#include "../include/globals.h"
#include "../include/display.h"
#include "../include/structures/point.h"
#include "../include/structures/line.h"
#include "../include/structures/polygon.h"
#include "../include/structures/matrix.h"
#include "../include/events/file.h"
#include "../include/events/input.h"
#include "../include/transformations/transform.h"
#include "../include/transformations/motion.h"

int main(int argc, char** argv) {
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Pain(t)");

    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(motion);
    glutMouseWheelFunc(wheel);

    glutMainLoop();

    destruction();

    return 0;
}