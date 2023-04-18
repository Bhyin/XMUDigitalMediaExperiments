#include <GL/glut.h>
#include <cmath>

/*利用正弦函数可以实现某个值随时间的周期变化
 * value = (A/2) * sin(omega * t + phi) + b;
 * value 是随时间变化的值，t 是时间
 * A 是value的变化幅度，等于value的最大值减去最小值
 * omega 等于 2π除以周期
 * phi 是相位
 * b用来确定value的值域, value 的值域为 [-(A/2) + b, (A/2) + b]
 * */

/*二维坐标的旋转可以用线性变换实现，只需要将坐标右乘上一个旋转矩阵即可
 * 设旋转的弧度为 theta， 则旋转矩阵为
 * [[cos(theta), -sin(theta)]
 *  [sin(theta), cos(theta)]]
 * */

/*二维坐标的缩放也可以用线性变换实现，只需要将坐标右乘上一个缩放矩阵即可
 * 设缩放倍数为 scale， 则缩放矩阵为
 * [[scale, 0    ]
 *  [0,     scale]]
 * */

// 常量π
const double pi = 3.14159265357989f;
// 全局变量
double r = 1.0f, g = 0.0f, b = 0.0f; // 镂垫的颜色
double theta = 0.0f; // 镂垫旋转的角度
double scale = 1.0f; // 镂垫缩放的倍数
double t = 0.0f; // 帧数（时间）

// 顶点
GLdouble vertices[3][2] = {
        {-1.0f, -1.0f},
        {1.0f,  -1.0f},
        {0.0f,  1.0f}
};

// 顶点位置
double positions[50000][2];

// 生成顶点位置
void generate_positions();

void Init(int *argc, char **argv);

void display();

void reshape(int w, int h);

void idlefunc();

void keyboard(unsigned char key, int x, int y) { if (key == 27)exit(0); }

int main(int argc, char **argv) {
    Init(&argc, argv);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    generate_positions();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idlefunc);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}

void generate_positions() {
    int j, k;
    GLdouble p[2] = {0.0f, 0.0f};

    for (k = 0; k < 50000; k++) {
        j = rand() % 3;
        p[0] = (p[0] + vertices[j][0]) / 2.0f;
        p[1] = (p[1] + vertices[j][1]) / 2.0f;

        positions[k][0] = p[0], positions[k][1] = p[1];
    }
}


void idlefunc() {

    // 颜色随时间周期变化，周期为60帧，幅度为0.0-1.0
    r = 0.5 * sin(pi / 30.0 * t) + 0.5;
    g = 0.5 * sin(pi / 30.0 * t + 1) + 0.5;
    b = 0.5 * sin(pi / 30.0 * t + 2) + 0.5;
    glColor3d(r, g, b);

    // 非线性变化，累计旋转角度theta等于时间乘以角速度, theta范围0-2*pi，周期是2秒即120帧
    theta = pi * sin(pi / 120.0 * t) + pi;

    // 线性变化
//    theta += (pi / 60);
//    if (theta > 2 * pi)theta = 0;

    // 大小随时间周期变化，周期为60帧，缩放幅度为0.5-1.5
    scale = 0.5 * sin(pi / 30.0 * t) + 1.0;

    t += 1.0;
    glutPostRedisplay();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    double cos_theta, sin_theta;
    glBegin(GL_POINTS);
    for (auto &pos: positions) {
        cos_theta = cos(theta), sin_theta = sin(theta);
        // 先后运用旋转矩阵和缩放矩阵
        glVertex2d((pos[0] * cos_theta - pos[1] * sin_theta) * scale,
                   (pos[0] * sin_theta + pos[1] * cos_theta) * scale
        );
    }
    glEnd();
    glFlush();
    glutSwapBuffers();
}


void reshape(int w, int h) {
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

void Init(int *argc, char **argv) {
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("exp02_task1");
}
