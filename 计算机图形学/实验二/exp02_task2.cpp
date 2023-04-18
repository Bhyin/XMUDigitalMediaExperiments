#include <GL/glut.h>
#include <cmath>

// 常量π
const double pi = 3.14159265357989f;
// 用于旋转动画的全局变量
double theta = 0.0f;

// 用于与相机交互的全局变量
// 相机交互标志，为真时可交互，否则不能交互
bool transform_flag = true;
// 分别控制相机的前进、右移与上升
double camera_forward = 2.0f;
double camera_right = 0.0f;
double camera_up = 0.0f;

// 控制相机的水平转动和垂直转动
double camera_horizontal_rotate = 0.0f;
double camera_vertical_rotate = 0.0f;

// 控制相机移动速度和旋转速度
double move_speed = 1.0f / 5.0f;
double rotate_speed = 1.0f / 10.0f;

// 立方体6个面共12个三角形的颜色
float colors[12][3] = {{1.0, 0.0, 0.0},
                       {0.5, 0.0, 0.0},
                       {1.0, 1.0, 0.0},
                       {0.5, 0.5, 0.0},
                       {0.0, 1.0, 0.0},
                       {0.0, 0.5, 0.0},
                       {0.0, 1.0, 1.0},
                       {0.0, 0.5, 0.5},
                       {0.0, 0.0, 1.0},
                       {0.0, 0.0, 0.5},
                       {1.0, 0.0, 1.0},
                       {0.5, 0.0, 0.5}};

// 正方体有8个定点
double positions[8][3] = {
        {0.5,  0.5,  0.5},// 第一卦限
        {-0.5, 0.5,  0.5},//第二卦限
        {-0.5, -0.5, 0.5},//第三卦限
        {0.5,  -0.5, 0.5},//第四卦限
        {0.5,  0.5,  -0.5},//第五卦限
        {-0.5, 0.5,  -0.5},//第六卦限
        {-0.5, -0.5, -0.5},//第七卦限
        {0.5,  -0.5, -0.5},//第八卦限
};

double positions_copy[8][3];

// 用于旋转的函数
// 立方体的坐标是三维向量，要使得立方体旋转需要用三维旋转矩阵
// 设沿 x、y、z 三个轴旋转的弧度分别为 rx, ry, rz, 那么得到的三维旋转矩阵(右手坐标系)就是
// 绕x轴旋转
// [[1          0           0      ]
//  [0          cos(rx)     -sin(rx)]
//  [0          sin(rx)     cos(rx)]]
// 绕y轴旋转
// [[cos(ry)    0           sin(ry) ]
//  [0          1           0       ]
//  [-sin(ry)   0           cos(ry) ]]
// 绕z轴旋转
// [[cos(rz)    -sin(rz)    0       ]
//  [sin(rz)    cos(rz)     0       ]
//  [0          0           1       ]]

// 绕x、y、z轴旋转angle个弧度
void rotate_x(const double *pos, double *res, double angle);

void rotate_y(const double *pos, double *res, double angle);

void rotate_z(const double *pos, double *res, double angle);

// 先绕x轴旋转angle_x个弧度，然后绕y/z轴旋转angle_y/angle_z个弧度
void rotate_xy(const double *pos, double *res, double angle_x, double angle_y);

void rotate_xz(const double *pos, double *res, double angle_x, double angle_z);

// 先绕x轴旋转angle_x， 再绕y轴旋转angle_y， 最后绕z轴旋转angle_z
void rotate_xyz(const double *pos, double *res, double angle_x, double angle_y, double angle_z);

void Init(int *argc, char **argv);

// 渲染立方体
void draw_cube(double positions[8][3]);

// 回调函数
void display();

void reshape(int w, int h);

void idle();

void keyboard(unsigned char key, int x, int y);

void mouseMotion(int x, int y);

int main(int argc, char **argv) {
    Init(&argc, argv);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(mouseMotion);

    glutMainLoop();
    return 0;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 不直接使用positions，防止立方体在多次线性变换的过程中，由于浮点数精度损失而造成剧烈形变
    for (int i = 0; i < 8; ++i)
        rotate_xyz(positions[i], positions_copy[i], theta, theta, theta);

    draw_cube(positions_copy);

    glFlush();
    glutSwapBuffers();
}

void draw_cube(double position[8][3]) {
    glBegin(GL_TRIANGLES);
    // 顶面 红色和暗红色
    glColor3fv(colors[0]);
    glVertex3dv(position[1]), glVertex3dv(position[2]), glVertex3dv(position[3]);
    glColor3fv(colors[1]);
    glVertex3dv(position[3]), glVertex3dv(position[0]), glVertex3dv(position[1]);

    // 底面 黄色和暗黄色
    glColor3fv(colors[2]);
    glVertex3dv(position[4]), glVertex3dv(position[7]), glVertex3dv(position[6]);
    glColor3fv(colors[3]);
    glVertex3dv(position[6]), glVertex3dv(position[5]), glVertex3dv(position[4]);

    // 前面 绿色和暗绿色
    glColor3fv(colors[4]);
    glVertex3dv(position[2]), glVertex3dv(position[6]), glVertex3dv(position[7]);
    glColor3fv(colors[5]);
    glVertex3dv(position[7]), glVertex3dv(position[3]), glVertex3dv(position[2]);

    // 后面 青色和暗青色
    glColor3fv(colors[6]);
    glVertex3dv(position[0]), glVertex3dv(position[4]), glVertex3dv(position[5]);
    glColor3fv(colors[7]);
    glVertex3dv(position[5]), glVertex3dv(position[1]), glVertex3dv(position[0]);

    // 左侧面 蓝色和暗蓝色
    glColor3fv(colors[8]);
    glVertex3dv(position[1]), glVertex3dv(position[5]), glVertex3dv(position[6]);
    glColor3fv(colors[9]);
    glVertex3dv(position[6]), glVertex3dv(position[2]), glVertex3dv(position[1]);

    // 右侧面 紫色和暗紫色
    glColor3fv(colors[10]);
    glVertex3dv(position[3]), glVertex3dv(position[7]), glVertex3dv(position[4]);
    glColor3fv(colors[11]);
    glVertex3dv(position[4]), glVertex3dv(position[0]), glVertex3dv(position[3]);

    glEnd();
}

void idle() {
    theta += (pi / 120.0);
    if (theta > 2 * pi)theta = 0;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 0.0, 10.0);
    gluLookAt(camera_right, camera_up, camera_forward,
              camera_horizontal_rotate, camera_vertical_rotate, 0.0,
              0.0, 1.0, 0.0);
    glutPostRedisplay();
}

void
mouseMotion(int x, int y) {
    static int last_x = 0, last_y = 0;
    if(transform_flag){
        if (x > last_x)camera_horizontal_rotate += rotate_speed;
        else if (x < last_x)camera_horizontal_rotate -= rotate_speed;
        // 由于窗口坐标和OpenGL坐标系y轴方向不同，所以当鼠标y值增加时，相机的水平旋转角度应该减小
        if (y > last_y)camera_vertical_rotate -= rotate_speed;
        else if (y < last_y)camera_vertical_rotate += rotate_speed;
    }
    last_x = x, last_y = y;
}


void
keyboard(unsigned char key, int x, int y) {
    // 切换交互状态
    if (key == 'L' || key == 'l')transform_flag = !transform_flag;
    else if (key == '\27')exit(0); // 退出

    if (transform_flag) {
        switch (key) {
            case 'W':// 前进
            case 'w':
                camera_forward -= move_speed;
                break;
            case 'A':// 左移
            case 'a':
                camera_right -= move_speed;
                camera_horizontal_rotate -= rotate_speed;
                break;
            case 'S':// 后退
            case 's':
                camera_forward += move_speed;
                break;
            case 'D':// 右移
            case 'd':
                camera_right += move_speed;
                camera_horizontal_rotate += rotate_speed;
                break;
            case 'Q': // 下降
            case 'q':
                camera_up -= move_speed;
                camera_vertical_rotate -= rotate_speed;
                break;
            case 'E': // 上升
            case 'e':
                camera_up += move_speed;
                camera_vertical_rotate += rotate_speed;
                break;
            default:
                break;
        }
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);

    double aspect = double(w) / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (aspect >= 1.0)glOrtho(-aspect, aspect, -1.0f, 1.0f, -2.0f, 2.0f);
    else glOrtho(-1.0f, 1.0f, -1 / aspect, 1 / aspect, -2.0f, 2.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glutPostRedisplay();
}

void Init(int *argc, char **argv) {
    glutInit(argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(600, 600);
    glutInitWindowPosition(0, 0);

    glutCreateWindow("exp02_task2");

    // 同时使用深度测试和反面剔除
    // 备注：glEnable函数必须再glutCreateWindow之后调用,因为OpenGL的context在create window之后才出现
    // 只开启深度测试仍然会出现消隐失败的情况，所以两个都要打开
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

//    if (!glIsEnabled(GL_DEPTH_TEST)) printf("Failed to enable GL_DEPTH_TEST");
}

void rotate_x(const double *pos, double *res, double angle) {
    double crx = cos(angle), srx = sin(angle);
    res[0] = pos[0];
    res[1] = pos[1] * crx - pos[2] * srx;
    res[2] = pos[1] * srx + pos[2] * crx;
}

void rotate_y(const double *pos, double *res, double angle) {
    double cry = cos(angle), sry = sin(angle);
    res[0] = pos[0] * cry + pos[2] * sry;
    res[1] = pos[1];
    res[2] = -pos[0] * sry + pos[2] * cry;
}

void rotate_z(const double *pos, double *res, double angle) {
    double crz = cos(angle), srz = sin(angle);
    res[0] = pos[0] * crz - pos[1] * srz;
    res[1] = pos[0] * srz + pos[1] * crz;
    res[2] = pos[2];
}

void rotate_xz(const double *pos, double *res, double angle_x, double angle_z) {
    double crx = cos(angle_x), crz = cos(angle_z),
            srx = sin(angle_x), srz = sin(angle_z);
    res[0] = pos[0] * crz - pos[1] * srz * crx + pos[2] * srz * srx;
    res[1] = pos[0] * srz + pos[1] * crz * crx - pos[2] * crz * srx;
    res[2] = pos[1] * srx + pos[2] * crx;
}

void rotate_xy(const double *pos, double *res, double angle_x, double angle_y) {
    double crx = cos(angle_x), cry = cos(angle_y),
            srx = sin(angle_x), sry = sin(angle_y);
    res[0] = pos[0] * cry + pos[1] * sry * srx + pos[2] * sry * crx;
    res[1] = pos[1] * crx - pos[2] * srx;
    res[2] = -pos[0] * sry + pos[1] * cry * srx + pos[2] * cry * crx;
}

void rotate_xyz(const double *pos, double *res, double angle_x, double angle_y, double angle_z) {
    double crx = cos(angle_x), cry = cos(angle_y), crz = cos(angle_z),
            srx = sin(angle_x), sry = sin(angle_y), srz = sin(angle_z);
    res[0] = pos[0] * crz * cry + pos[1] * (crz * sry * srx - srz * crx) + pos[2] * (crz * sry * crx + srz * srx);
    res[1] = pos[0] * srz * cry + pos[1] * (srz * sry * srx + crz * crx) + pos[2] * (srz * sry * crx - crz * srx);
    res[2] = -pos[0] * sry * +pos[1] * cry * srx + pos[2] * cry * crx;
}
