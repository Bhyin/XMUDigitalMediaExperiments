#include <GL/glut.h>
#include <cmath>
#include <iostream>
using namespace std;

const GLdouble pi = 3.14159265357989;
const GLdouble tau = 2 * pi;

//取样数目
const int N = 2000;
const int DotNum = (N + 1) * 2;

// 内圈半径
const GLdouble inner_radius = 0.24;
//外圈半径
const GLdouble outer_radius = 1.2 * inner_radius;
const GLdouble horizontal_buff = 2.6 * inner_radius;
const GLdouble vertical_buff = inner_radius;

double custom_degree = tau / N;

//存储单个圆环的的顶点，圆环的圆心是原点
double positions[DotNum][2];

//五环的颜色
struct Color {
    float r, g, b;
}
        RED{220 / 255.0, 47 / 255.0, 31 / 255.0},
        BLUE{0 / 255.0, 107 / 255.0, 176 / 255.0},
        BLACK{29 / 255.0, 24 / 255.0, 21 / 255.0},
        YELLOW{239 / 255.0, 169 / 255.0, 13 / 255.0},
        GREEN{5 / 255.0, 147 / 255.0, 65 / 255.0};

//五环的相对位置
struct RingPos {
    GLdouble x, y;
}
        BlueR{-horizontal_buff, 0.5 * vertical_buff},
        BlackR{0, 0.5 * vertical_buff},
        RedR{horizontal_buff, 0.5 * vertical_buff},
        YellowR{-1.3 * inner_radius, -0.5 * vertical_buff},
        GreenR{1.3 * inner_radius, -0.5 * vertical_buff};

void init(int *argc, char **argv);

void polar2cartesian(const GLdouble &rho, const GLdouble &theta, GLdouble *x, GLdouble *y);

void generate_position();

//绘制positions[start_idx]~positions[end_idx]这些顶点，模式为相连的三角形条带，
// pos是圆环的相对位置，color是圆环的颜色
void draw_single_ring(int start_idx, int end_idx, RingPos &pos, Color &color);

// 分段render每个环
void
display() {
    glClear(GL_COLOR_BUFFER_BIT);

    //1 渲染出蓝色环从45度到315度的部分
    draw_single_ring(DotNum / 8 - 1, DotNum * 7 / 8, BlueR, BLUE);
    //2 渲染出黑色环从135度到225度的部分
    draw_single_ring(DotNum * 3 / 8 - 1, DotNum * 5 / 8, BlackR, BLACK);
    //3 渲染出整个黄色环
    draw_single_ring(0, DotNum, YellowR, YELLOW);
    //4 渲染出蓝色环的0度到45度、315度到360度的部分
    draw_single_ring(0, DotNum / 8, BlueR, BLUE);
    draw_single_ring(DotNum * 7 / 8 - 1, DotNum, BlueR, BLUE);
    //5 渲染出红色环135度到225度的部分
    draw_single_ring(DotNum * 3 / 8 - 1, DotNum * 5 / 8, RedR, RED);
    //6 渲染出黑色环225度到315度的部分
    draw_single_ring(DotNum * 5 / 8 - 1, DotNum * 7 / 8, BlackR, BLACK);
    //7 渲染出整个绿色环
    draw_single_ring(0, DotNum, GreenR, GREEN);
    //8 渲染出黑色环0度到135度，315度到360度的部分
    draw_single_ring(DotNum * 7 / 8 - 1, DotNum, BlackR, BLACK);
    draw_single_ring(0, DotNum * 3 / 8, BlackR, BLACK);
    //9 渲染出红色环0度到135度，225度到360度的部分
    draw_single_ring(0, DotNum * 3 / 8, RedR, RED);
    draw_single_ring(DotNum * 5 / 8 - 1, DotNum, RedR, RED);

    glFlush();
}

void reshape(int w, int h);

int
main(int argc, char **argv) {
    init(&argc, argv);

    glClearColor(255, 255, 255, 1.0);

    generate_position();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}

void
init(int *argc, char **argv) {
    glutInit(argc, argv);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);

    glutCreateWindow("simple");
}

void
reshape(int w, int h) {
    // 视口
    glViewport(0, 0, w, h);

    // 宽高比
    double aspect = (double) w / h;

    // 投影矩阵
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 修改投影矩阵， 始终保持宽高比（right - left）/ （top - bottom）等于 aspect
    if (aspect >= 1)glOrtho(-aspect, aspect, -1, 1, 0, 1);
    else glOrtho(-1, 1, -1 / aspect, 1 / aspect, 0, 1);

    glMatrixMode(GL_MODELVIEW);

    // 重绘
    glutPostRedisplay();
}


void polar2cartesian(const GLdouble &rho, const GLdouble &theta, GLdouble *x, GLdouble *y) {
    *x = rho * cos(theta);
    *y = rho * sin(theta);
}

void
generate_position() {
    GLdouble x, y;
    for (int i = 0; i < N; ++i) {
        //外圈的坐标
        polar2cartesian(outer_radius, (i - 0.5) * custom_degree, &x, &y);
        positions[2 * i][0] = x, positions[2 * i][1] = y;
        //内圈的坐标
        polar2cartesian(inner_radius, i * custom_degree, &x, &y);
        positions[2 * i + 1][0] = x, positions[2 * i + 1][1] = y;
    }

    //首尾相连
    positions[2 * N][0] = positions[0][0], positions[2 * N][1] = positions[0][1];
    positions[2 * N + 1][0] = positions[1][0], positions[2 * N + 1][1] = positions[1][1];
}

void
draw_single_ring(int start_idx, int end_idx, RingPos &pos, Color &color) {
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = start_idx; i <= end_idx; i++)
        glVertex2d(positions[i][0] + pos.x, positions[i][1] + pos.y);
    glEnd();
}