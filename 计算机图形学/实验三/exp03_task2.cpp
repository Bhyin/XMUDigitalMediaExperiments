#include <GL/glut.h>
#include "file_ops.h"
#include "utils.h"
#include "cmath"

int main(int argc, char **argv) {
    Init(argc, argv);
    Window("exp03_task1", 720, 1280);
    Enable(GL_DEPTH_TEST);
    Enable(GL_CULL_FACE);

    // 启用光源
    Enable(GL_LIGHTING);
    Enable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);

    // 投影矩阵
    Ortho(-1000.0, 1000.0, -1500.0, 1500.0, -1000.0, 1000.0);

    // 读取模型数据
    static PlyModel model;
    model.Load("lizhenxiout.ply");

    // 回调函数
    glutDisplayFunc([]() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 设置顶点法向
        NormalPointer(GL_DOUBLE, 6 * sizeof(double), model.GetVertices() + 3);
        // 使用顶点数组，而不是glBegin和glEnd
        VertexPointer(3, GL_DOUBLE, 6 * sizeof(double), model.GetVertices());
        // 用索引数组渲染
        DrawElements(GL_TRIANGLES, model.FaceNum() * 3, GL_UNSIGNED_INT, model.GetFaces());

        glFlush();
        glutSwapBuffers();
    });

    // 光源旋转角度和光源位置
    static double angle = 0;
    static float light_position[4]{0.0, 0.0, 0.0, 1.0};
    glutIdleFunc([]() {
        angle += degree;
        if (angle > tau)angle -= tau;

        // 让光源绕着y轴以椭圆轨迹旋转
        light_position[0] = 1000.0 * cos(angle);
        light_position[2] = 1500.0 * sin(angle);

        // 刷新光源位置
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glutPostRedisplay();
    });

    glutMainLoop();
    return 0;
}




