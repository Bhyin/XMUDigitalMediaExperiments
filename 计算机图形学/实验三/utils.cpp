//
// Created by Bhyin on 4/30/2023.
//

#include "./utils.h"
#include <GL/glut.h>
#include <GL/glu.h>

void load_modelview() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void load_projection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void reshape_callback_function(int w, int h) {
    glViewport(0, 0, w, h);

    double aspect = double(w) / h;

    if (aspect >= 1.0)Ortho(-aspect, aspect, -1.0f, 1.0f, -2.0f, 2.0f);
    else Ortho(-1.0f, 1.0f, -1 / aspect, 1 / aspect, -2.0f, 2.0f);

    load_modelview();

    glutPostRedisplay();
}

// initialize
void Init(int argc, char **argv) {
    glutInit(&argc, argv);
}

// CreateWindow
int Window(
        const char *title,
        int width,
        int height,
        int x,
        int y
) {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(x, y);
    return glutCreateWindow(title);
}

// Enable OpenGL features
bool Enable(unsigned int feature) {
    glEnable(feature);
    return glIsEnabled(feature);
}

void VertexPointer(int size, unsigned int type, int stride, const void *pointer) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(size, type, stride, pointer);
}

void ColorPointer(int size, unsigned int type, int stride, const void *pointer) {
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(size, type, stride, pointer);
}

void NormalPointer(unsigned int type, int stride, const void *pointer) {
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(type, stride, pointer);
}

void DrawElements(unsigned int primitive, int count, unsigned int type, const void *indices) {
    glEnableClientState(GL_INDEX_ARRAY);
    glDrawElements(primitive, count, type, indices);
}


// Projection
void Ortho(double l, double r, double b, double t, double n, double f) {
    load_projection();
    glOrtho(l, r, b, t, n, f);
}

// Camera
void LookAt(double ex, double ey, double ez,
            double ax, double ay, double az,
            double ux, double uy, double uz) {
    load_modelview();
    gluLookAt(ex, ey, ez, ax, ay, az, ux, uy, uz);
}

// ClearColor
void ClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

// Material
void SetMaterial(const Material &material) {
// 设置材质属性
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material.ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material.diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material.specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material.shininess);
}

// Lighting
void SetLighting(const Lighting &lighting) {
    // 设置光照属性
    glLightfv(GL_LIGHT0, GL_AMBIENT, lighting.ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lighting.diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lighting.specular);
}





