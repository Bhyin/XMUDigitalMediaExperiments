#include <GL/glut.h>
#include <cmath>

const GLdouble pi = 3.14159265357989;
const GLdouble tau = 2 * pi;

const GLint N = 360;
const GLint color_num = 7;

const GLdouble radius = 1.0;

const GLint M = N / color_num + 1;

int offset = 0;

GLdouble positions[N + 1][2];
GLdouble colors[7][3] = {
        {1, 0,   0},
        {1, 0.5, 0},
        {1, 1,   0},
        {0, 1,   0},
        {0, 1,   1},
        {0, 0,   1},
        {1, 0,   1},
};

void polar2cartesian(const GLdouble &rho, const GLdouble &theta, GLdouble *x, GLdouble *y);

void generate_position();


void
time_updater(int value) {
    glutPostRedisplay();
    glutTimerFunc(2, time_updater, 1);

    if (offset > N) offset = 0;
    else offset += 1;

    glutPostRedisplay();
}

void
display() {
    glClear(GL_COLOR_BUFFER_BIT);

    int color_idx;


    for (int i = 0; i < N; ++i) {
        color_idx = ((i - offset + N) % N) / M;
        glColor3d(colors[color_idx][0], colors[color_idx][1], colors[color_idx][2]);

        glBegin(GL_TRIANGLES);
        glVertex2d(0.0f, 0.0f);
        glVertex2d(positions[i][0], positions[i][1]);
        glVertex2d(positions[i + 1][0], positions[i + 1][1]);
        glEnd();
    }

    glFlush();
    glutSwapBuffers();
}

void init(int *argc, char **argv);

int main(int argc, char **argv) {
    init(&argc, argv);

    generate_position();

    glutDisplayFunc(display);
    glutTimerFunc(100, time_updater, 0);

    glutMainLoop();
    return 0;
}

void
init(int *argc, char **argv) {
    glutInit(argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);

    glutCreateWindow("exp01");
}

void polar2cartesian(const GLdouble &rho, const GLdouble &theta, GLdouble *x, GLdouble *y) {
    *x = rho * cos(theta);
    *y = rho * sin(theta);
}

void generate_position() {
    GLdouble x, y;
    for (int i = 0; i < N; i++) {
        polar2cartesian(radius, i * tau / N, &x, &y);
        positions[i][0] = x, positions[i][1] = y;
    }
    positions[N][0] = positions[0][0], positions[N][1] = positions[0][1];
}
