//
// Created by Bhyin on 5/1/2023.
//

#include "Transform.h"
#include <GL/glut.h>

extern void load_modelview();

void Translate(double x, double y, double z) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(x, y, z);
}

void Rotate(double x, double y, double z) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotated(x, 1.0, 0.0, 0.0);
    glRotated(y, 0.0, 1.0, 0.0);
    glRotated(z, 0.0, 0.0, 1.0);
}

void Scale(double x, double y, double z) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScaled(x, y, z);
}