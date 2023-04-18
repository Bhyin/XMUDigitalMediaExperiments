#include <GL/glut.h>
#include <cmath>

// 常量
const double pi = 3.14159265357989f;
const double radius = 0.5f;

// 球坐标系
const double latitude_angle = pi / 16.0; // 180度分成16份
const double longitude_angle = pi / 16.0;// 360度分成32份

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

double theta = pi / 4;

// 线框的原始数据
// 北极和南极单独存储
double north_polar[3] = {0.0, 0.0, radius};
double south_polar[3] = {0.0, 0.0, -radius};
// 三维数组，每一行表示同一纬度，每一列表示统一经度
double positions[15][32][3];

// 用于对线框球体进行变换的辅助变量
double north_polar_copy[3];
double south_polar_copy[3];
double positions_copy[15][32][3];

void rotate_xy(const double *pos, double *res, double angle_x, double angle_y);

void Init(int *argc, char **argv);

void generate_positions();

void draw_gizmo(double ball[15][32][3], double north[3], double south[3]);

void display();

void idle();

void reshape(int w, int h);

void keyboard(unsigned char key, int x, int y);

void mouseMotion(int x, int y);


int main(int argc, char **argv) {

    Init(&argc, argv);
    generate_positions();

    glClearColor(0, 0, 0, 1);

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(mouseMotion);

    glutMainLoop();
    return 0;
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
display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0, 0.0, 0.0);

    for (int i = 0; i < 15; ++i)
        for (int j = 0; j < 32; ++j) {
            rotate_xy(north_polar, north_polar_copy, theta, theta);
            rotate_xy(positions[i][j], positions_copy[i][j], theta, theta);
            rotate_xy(south_polar, south_polar_copy, theta, theta);
        }
    draw_gizmo(positions_copy, north_polar_copy, south_polar_copy);

    glFlush();
    glutSwapBuffers();
}

void
draw_gizmo(double ball[15][32][3], double north[3], double south[3]) {
    // 画纬线
    for (int i = 0; i < 15; ++i) {
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < 32; ++j)glVertex3dv(ball[i][j]);
        glEnd();
    }

    // 画经线
    for (int j = 0; j < 32; j++) {
        glBegin(GL_LINE_STRIP);
        glVertex3dv(north);
        for (int i = 0; i < 15; ++i) glVertex3dv(ball[i][j]);
        glVertex3dv(south);
        glEnd();
    }
}

void
reshape(int w, int h) {
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

void generate_positions() {
    double la_angle, lo_angle, clo, cla, slo, sla;
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 32; ++j) {
            la_angle = (i + 1) * latitude_angle;
            lo_angle = j * longitude_angle;
            cla = cos(la_angle), sla = sin(la_angle);
            clo = cos(lo_angle), slo = sin(lo_angle);
            positions[i][j][0] = radius * sla * clo;
            positions[i][j][1] = radius * sla * slo;
            positions[i][j][2] = radius * cla;
        }
    }
}

void
Init(int *argc, char **argv) {
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("exp02_task3");

    glEnable(GL_DEPTH_TEST);
}

void
rotate_xy(const double *pos, double *res, double angle_x, double angle_y) {
    double crx = cos(angle_x), cry = cos(angle_y),
            srx = sin(angle_x), sry = sin(angle_y);
    res[0] = pos[0] * cry + pos[1] * sry * srx + pos[2] * sry * crx;
    res[1] = pos[1] * crx - pos[2] * srx;
    res[2] = -pos[0] * sry + pos[1] * cry * srx + pos[2] * cry * crx;
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

void
mouseMotion(int x, int y) {
    static int last_x = 0, last_y = 0;
    if (transform_flag) {
        if (x > last_x)camera_horizontal_rotate += rotate_speed;
        else if (x < last_x)camera_horizontal_rotate -= rotate_speed;
        // 由于窗口坐标和OpenGL坐标系y轴方向不同，所以当鼠标y值增加时，相机的水平旋转角度应该减小
        if (y > last_y)camera_vertical_rotate -= rotate_speed;
        else if (y < last_y)camera_vertical_rotate += rotate_speed;
    }
    last_x = x, last_y = y;
}
