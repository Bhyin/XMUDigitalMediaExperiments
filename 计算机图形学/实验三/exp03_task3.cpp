#include <GL/glut.h>
#include "utils.h"
#include "Transform.h"
#include <cmath>


// 材质
// 黄铜
Material BrassMaterial = {{0.33, 0.22, 0.03, 1.0},
                          {0.78, 0.22, 0.03, 1.0},
                          {0.99, 0.91, 0.81, 1.0},
                          27.8};
// 红色塑料
Material RedPlasticMaterial = {{0.3, 0.0, 0.0, 1.0},
                               {0.6, 0.0, 0.0, 1.0},
                               {0.8, 0.6, 0.6, 1.0},
                               32.0};
// 白色光照
Material WhiteShinyMaterial = {{1.0, 1.0, 1.0, 1.0},
                               {1.0, 1.0, 1.0, 1.0},
                               {1.0, 1.0, 1.0, 1.0},
                               100.0};


// 光照
// 白色光
Lighting WhiteLight = {{0.0, 0.0, 0.0, 1.0},
                       {1.0, 1.0, 1.0, 1.0},
                       {1.0, 1.0, 1.0, 1.0}};
// 彩色光
Lighting ColoredLight = {{0.2, 0.0, 0.0, 1.0},
                         {0.0, 1.0, 0.0, 1.0},
                         {0.0, 0.0, 1.0, 1.0}};

// 顶点位置,正方体中心与世界坐标原点重合，直接用顶点坐标作为法向量
double vertices[3 * 8] = {
        0.5, 0.5, 0.5,
        -0.5, 0.5, 0.5,
        -0.5, -0.5, 0.5,
        0.5, -0.5, 0.5,
        0.5, 0.5, -0.5,
        -0.5, 0.5, -0.5,
        -0.5, -0.5, -0.5,
        0.5, -0.5, -0.5,
};

unsigned int indices[4 * 6] = {
        0, 1, 2, 3, // 顶面
        5, 4, 7, 6, // 底面
        3, 2, 6, 7, // 前面
        1, 0, 4, 5, // 后面
        2, 1, 5, 6, // 左侧面
        0, 3, 7, 4 // 右侧面
};

int main(int argc, char **argv) {
    Init(argc, argv);
    Window("exp03_task3", 720, 720);
    Enable(GL_DEPTH_TEST);
    Enable(GL_CULL_FACE);

    // 启用光源
    Enable(GL_LIGHTING);
    Enable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);

    // 光源位置
    static float light_position[4] = {1.0, 1.0, 1.0, 1.0};

    // 设置光源位置
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // 设置初始材质
    SetMaterial(BrassMaterial);
    SetLighting(WhiteLight);

    // 使用顶点数组渲染，而不是glBegin和glEnd

    // 使用顶点数组后，回调函数可以非常简短，直接用C++的lambda表达式传递回调函数
    glutDisplayFunc([]() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 顶点数组
        VertexPointer(3, GL_DOUBLE, 3 * sizeof(double), vertices);
        // 法向
        NormalPointer(GL_DOUBLE, 3 * sizeof(double), vertices);
        // 用索引数组绘制
        DrawElements(GL_QUADS, 4 * 6, GL_UNSIGNED_INT, indices);

        glFlush();
        glutSwapBuffers();
    });

    static double theta = 0.0;
    // 空闲回调函数
    glutIdleFunc([]() {
        theta += degree;
        if (theta > 2 * pi)theta = 0;
        // 将GL的glRotated封装起来
        Rotate(theta / degree, theta / degree, theta / degree);
        glutPostRedisplay();
    });

    glutKeyboardFunc([](unsigned char key, int x, int y) {
        switch (key) {
            default:
                SetMaterial(BrassMaterial);
                SetLighting(WhiteLight);
                break;
            case 'b':
            case 'B':
                SetMaterial(BrassMaterial);
                break;
            case 'n':
            case 'N':
                SetMaterial(RedPlasticMaterial);
                break;
            case 'm':
            case 'M':
                SetMaterial(WhiteShinyMaterial);
                break;
            case 'o':
            case 'O':
                SetLighting(WhiteLight);
                break;
            case 'p':
            case 'P':
                SetLighting(ColoredLight);
                break;
            case 'q':
            case 'Q':
                exit(0);
        }
    });

    glutMainLoop();

    return 0;
}