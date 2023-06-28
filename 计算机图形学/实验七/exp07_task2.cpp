#include "GL/glut.h"

#include "Bazier.h"
#include "utils.h"

const int SamplesNum = 200;

double handles3x3[3 * 3 * 3]{
	-0.5,0.5,-1.0,
	-0.5,0.0,-0.6,
	-0.5,-0.5,0.0,

	0.0,0.5,-1.0,
	0.0,0.0,-0.6,
	0.0,-0.5,0.0,

	0.5,0.5,-1.0,
	0.5,0.0,-0.6,
	0.5,-0.5,0.0,
};

double handles4x4[4 * 4 * 3]{
	-0.6,0.6,-1.0,
	-0.6,0.2,-0.6,
	-0.6,-0.2,0.2,
	-0.6,-0.6,0.5,

	-0.2,0.6,-1.0,
	-0.2,0.2,-0.6,
	-0.2,-0.2,0.2,
	-0.2,-0.6,0.5,

	0.2,0.6,-1.0,
	0.2,0.2,-0.6,
	0.2,-0.2,0.2,
	0.2,-0.6,0.5,

	0.6,0.6,-1.0,
	0.6,0.2,-0.6,
	0.6,-0.2,0.2,
	0.6,-0.6,0.5,
};

double handles5x5[5 * 5 * 3]{
	-0.8,0.8,-1.0,
	-0.8,0.4,-0.9,
	-0.8,0.0,-0.5,
	-0.8,-0.4,-0.3,
	-0.8,-0.8,0.8,

	-0.4,0.8,-1.0,
	-0.4,0.4,-0.9,
	-0.4,0.0,-0.5,
	-0.4,-0.4,-0.3,
	-0.4,-0.8,0.8,

	-0.0,0.8,-1.0,
	-0.0,0.4,-0.9,
	-0.0,0.0,-0.5,
	-0.0,-0.4,-0.3,
	-0.0,-0.8,0.8,

	0.4,0.8,-1.0,
	0.4,0.4,-0.9,
	0.4,0.0,-0.5,
	0.4,-0.4,-0.3,
	0.4,-0.8,0.8,

	0.8,0.8,-1.0,
	0.8,0.4,-0.9,
	0.8,0.0,-0.5,
	0.8,-0.4,-0.3,
	0.8,-0.8,0.8,
};


int frame = 0;
bool enable_animation = true;
Bhyin::BazierSurface surface3x3 = Bhyin::BazierSurface(handles3x3, 3, 3, SamplesNum);
Bhyin::BazierSurface surface4x4 = Bhyin::BazierSurface(handles4x4, 4, 4, SamplesNum);
Bhyin::BazierSurface surface5x5 = Bhyin::BazierSurface(handles5x5, 5, 5, SamplesNum);

Bhyin::BazierSurface surface[3]{ surface3x3, surface4x4,surface5x5 };
int ActiveIdx = 0;


void DrawSurface();
void DrawTips();

int main(int argc, char** argv)
{
	Init(argc, argv);
	Window("exp07_task2", 800, 800);
	Enable(GL_DEPTH_TEST);
	

	glutDisplayFunc([]() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawSurface();
		DrawTips();

		glFlush();
		glutSwapBuffers();
		});

	// 鼠标点击
	glutMouseFunc([](int button, int state, int x, int y) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) // 左键按下
		{
			double posX, posY;
			WindowToOpenGL(x, y, &posX, &posY);
			
			// 检测某一个点是否被鼠标选中
			/*用二维的鼠标坐标去寻找三维空间的控制点求交，需要射线检测的技术，不好弄所以忽略深度*/
			surface[ActiveIdx].isHandleDragged(posX, posY);
			glutPostRedisplay(); // 重绘窗口
		}
		else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) // 左键松开
		{
			double posX, posY;
			WindowToOpenGL(x, y, &posX, &posY);

			// 如果某个顶点被拖动，就修改顶点的位置,并结束拖动状态
			if (surface[ActiveIdx].isDragging)
			{
				surface[ActiveIdx].Drag(posX, posY);
				surface[ActiveIdx].isDragging = false;
				surface[ActiveIdx].dragRowIndex = -1;
				surface[ActiveIdx].dragColIndex = -1;
				surface[ActiveIdx].RefreshSurface();
			}
		}
		});

	// 鼠标移动
	glutMotionFunc([](int x, int y) {
		double posX, posY;
		WindowToOpenGL(x, y, &posX, &posY);

		// 如果有点被拖动，就实时修改其位置
		if (surface[ActiveIdx].isDragging)
		{	
			surface[ActiveIdx].Drag(posX, posY);
			surface[ActiveIdx].RefreshSurface();
		}
		});

	// 按键函数
	glutKeyboardFunc([](unsigned char key, int x, int y) {
		switch (key) {
		case 27:
			exit(0);
		case 'a':
		case 'A': // A键修改动画状态
			enable_animation = !enable_animation;
			break;
		case 'q':
		case 'Q':
			ActiveIdx = 0;
			break;
		case 'w':
		case 'W':
			ActiveIdx = 1;
			break;
		case 'e':
		case 'E':
			ActiveIdx = 2;
			break;
		}
		});


	// 空闲调用
	glutIdleFunc([]() {
		if (enable_animation) // 如果启用动画
		{
			frame += 1;
			if (surface[ActiveIdx].GetSamplesNum() <= frame)frame = 0;
		}
		else frame = surface[ActiveIdx].GetSamplesNum() - 1; // 显示完整曲面

		glutPostRedisplay();
		});

	// 使用utils里的reshape回调函数
	glutReshapeFunc(DefaultReshape);


	glutMainLoop();
	return 0;
}

void DrawSurface()
{
	// 绘制曲线
	glPointSize(20);
	glLineWidth(4);
	for (int i = 0; i < surface[ActiveIdx].GetHandlesRowNum(); ++i)
	{
		glColor3f(1.0, 0.0, 0.0);
		VertexPointer(3, GL_DOUBLE, 0, surface[ActiveIdx].GetHandlesData() + i * surface[ActiveIdx].GetHandlesColNum() * 3);
		DrawArrays(GL_POINTS, 0, surface[ActiveIdx].GetHandlesColNum());
		DrawArrays(GL_LINE_STRIP, 0, surface[ActiveIdx].GetHandlesColNum());
		glColor3f(0.0, 1.0, 0.0);
		VertexPointer(3, GL_DOUBLE, 0, surface[ActiveIdx].GetCurvesData() + i * surface[ActiveIdx].GetSamplesNum() * 3);
		DrawArrays(GL_LINE_STRIP, 0, surface[ActiveIdx].GetSamplesNum());
	}
	
	glColor3f(1.0, 1.0, 1.0);
	// 绘制曲面
	for (int i = 0; i < frame - 1; ++i) // 画frame根线，构成一个面
	{
		VertexPointer(3, GL_DOUBLE, 0, surface[ActiveIdx].GetSurfaceData() + i * surface[ActiveIdx].GetSamplesNum() * 3);
		DrawArrays(GL_LINE_STRIP, 0, surface[ActiveIdx].GetSamplesNum());
	}
	glColor3d(0.0, 0.0, 1.0);
	glLineWidth(4);
	VertexPointer(3, GL_DOUBLE, 0, surface[ActiveIdx].GetSurfaceData() + frame * surface[ActiveIdx].GetSamplesNum() * 3);
	DrawArrays(GL_LINE_STRIP, 0, surface[ActiveIdx].GetSamplesNum());
}

void DrawTips()
{
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(-1.0, 0.9);
	for (char c : "Press the left mouse button. Drag the red point")glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	glRasterPos2f(-1.0, 0.8);
	for (char c : "Press 'q' 'w', 'e' to switch between different faces")glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	glRasterPos2f(-1.0, 0.7);
	for (char c : "Press 'a': enable/disable animation.")glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	glRasterPos2f(-1.0, 0.6);
	if (enable_animation)for (char c : "Animation State: Enable.")glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	else for (char c : "Animation State: Disable.")glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);

}

