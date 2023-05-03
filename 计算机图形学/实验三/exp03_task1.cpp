#include <GL/glut.h>
#include "file_ops.h"
#include "utils.h"

int main(int argc, char **argv) {
    Init(argc, argv);
    Window("exp03_task1", 720, 1280);
    Enable(GL_DEPTH_TEST);
    Enable(GL_CULL_FACE);

    // 调整投影矩阵
    Ortho(-1000.0, 1000.0, -1500.0, 1500.0, -1000.0, 1000.0);

    // 读取模型数据
    static PlyModel model;
    model.Load("lizhenxiout.ply");

    // 回调函数
    glutDisplayFunc([]() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        // 使用法向量设置顶点颜色
//        ColorPointer(3, GL_DOUBLE, 6 * sizeof(double), model.GetVertices() + 3);

        // 设置纯色
        glColor3f(1.0, 0.0, 0.0);
        // 使用顶点数组，而不是glBegin和glEnd
        VertexPointer(3, GL_DOUBLE, 6 * sizeof(double), model.GetVertices());
        // 用索引数组渲染
        DrawElements(GL_TRIANGLES, model.FaceNum() * 3, GL_UNSIGNED_INT, model.GetFaces());

        glFlush();
        glutSwapBuffers();
    });

    glutMainLoop();
    return 0;
}



