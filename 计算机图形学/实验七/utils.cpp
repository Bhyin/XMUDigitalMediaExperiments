//
// Created by Bhyin on 4/30/2023.
//

#include "utils.h"
#include "GL/glut.h"
#include <GL/glu.h>

void load_modelview() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void load_projection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void DefaultReshape(int w, int h) {
    glViewport(0, 0, w, h);

    double aspect = double(w) / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (aspect >= 1.0)glOrtho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    else glOrtho(-1.0f, 1.0f, -1 / aspect, 1 / aspect, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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

void DrawArrays(unsigned int mode, int first, int count)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(mode, first, count);
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


void WindowToOpenGL(int x, int y, double* ox, double* oy)
{

    int width = glutGet(GLUT_WINDOW_WIDTH); // 获取窗口宽度
    int height = glutGet(GLUT_WINDOW_HEIGHT); // 获取窗口高度
    double aspect = double(width) / height;

    /*为了保证窗口变形时，曲线不会变形，需要一个reshape回调函数对glOrtho进行处理，
    为了保证鼠标点击的位置能映射到正确的位置，坐标转化也需要做类似处理*/
    if (aspect >= 1.0)
    {
        *ox = (double(x) / width * 2.0f - 1.0f) * aspect; // 将x坐标映射到[-aspect, aspect]区间
        *oy = 1.0f - double(y) / height * 2.0f; // 将y坐标映射到[-1, 1]区间，并反转方向
    }
    else
    {
        *ox = double(x) / width * 2.0f - 1.0f; // 将x坐标映射到[-1, 1]区间
        *oy = (1.0f - double(y) / height * 2.0f) / aspect; // 将y坐标映射到[-1/aspect, 1/aspect]区间，并反转方向
    }
}





