#define _CRT_SECURE_NO_WARNINGS 1

#include "GL/glut.h"
#include "Bazier.h"
#include "utils.h" // 实验三编写的utils
#include <cmath>

// 曲线的采样点数量
const int SampleNum = 200;

// 颜色，用于区分不同层次的控制点
float colors[14 * 3]{
	1.0,0.0,0.0,
	0.0,1.0,0.0,
	0.0,0.0,1.0,
	1.0,1.0,0.0,
	1.0,0.0,1.0,
	0.0,1.0,1.0,
	1.0,0.5,0.5,

	0.5,1.0,0.5,
	0.5,0.5,1.0,
	1.0,1.0,0.5,
	1.0,0.5,1.0,
	0.5,1.0,1.0,
	0.5,0.5,0.5,
	0.75,0.45,0.11,
};
// 获取颜色数组的指针
inline float* GetColorPtr(int i) { return colors + 3 * ((i + 14) % 14); }

// 窗口文字提示
const char AddPoint[] = "Click left mouse button to append point.";
const char ChangeState[] = "Press 'A': enable/disable animation.";
const char ClearScreen[] = "Press 'C': clear screen.";
const char EnableState[] = "Animation State: Enable.";
const char DisableState[] = "Animation State: Disable";
const char KeyState[] = "Press Key: ";
char KeyChar = '-';

// 用于动画的参数
float frame_ratio = 0.5f;
bool enable_animation = true;

// 曲线对象
/*
初始没有任何曲线，鼠标点击窗口可以创建控制点，按下键盘上的 ‘c’ 键可以清空屏幕
*/
Bhyin::BazierCurve curve = Bhyin::BazierCurve();


// 在屏幕左上角显示字符串
void DisplayTips();

// 绘制贝塞尔曲线
void DrawBazierCurve(float ratio);

int main(int argc, char** argv)
{
	Init(argc, argv);
	Window("exp07_task1", 800, 800);

	glShadeModel(GL_SMOOTH);
	
	glutDisplayFunc([]() {
		glClear(GL_COLOR_BUFFER_BIT);

		if(curve.exist)DrawBazierCurve(frame_ratio); // 绘制曲线
		DisplayTips(); // 绘制文字

		glFlush();
		glutSwapBuffers();
		});

	// 鼠标移动
	glutMotionFunc([](int x, int y) {
		//frame_ratio = 0.0;
		double posX, posY;
		WindowToOpenGL(x, y, &posX, &posY);

		// 如果有点被拖动，就实时修改其位置
		if (curve.isDragging)
		{
			curve.Drag(posX, posY);
			curve.RefreshCurve();
		}
		});

	// 鼠标点击
	glutMouseFunc([](int button, int state, int x, int y) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) // 左键按下
		{
			KeyChar = '0';
			/*frame_ratio = 0.0;*/
			double posX, posY;
			WindowToOpenGL(x, y, &posX, &posY);
			//printf("%d, %d, %f, %f \n",x, y, posX, posY);
			
			// 检测某一个点是否被鼠标选中
			curve.isHandleDragged(posX, posY);
			glutPostRedisplay(); // 重绘窗口
		}
		else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) // 左键松开
		{
			double posX, posY;
			WindowToOpenGL(x, y, &posX, &posY);

			// 如果某个顶点被拖动，就修改顶点的位置,并结束拖动状态
			if (curve.isDragging)
			{
				curve.Drag(posX, posY);
				curve.isDragging = false;
				curve.dragIndex = -1;
				curve.RefreshCurve();
			}
			else
			{
				// 没选中，那就在此处添加控制点
				curve.AppendHandle(posX, posY, 0.0);
			}
		}
		});

	// 空闲调用
	glutIdleFunc([]() {
		if (enable_animation) // 如果启用动画
		{
			frame_ratio += 1 / 240.0; // 如果动画太快，可以把分母改大一点
			if (frame_ratio >= 1.0)frame_ratio = 0.0;
		}
		else frame_ratio = 1.0; // 显示完整曲线
		
		glutPostRedisplay();
		});

	// 按键函数
	glutKeyboardFunc([](unsigned char key, int x, int y) {
		KeyChar = key;
		switch (key) {
		case 27:
			exit(0);
		case 'c':
		case 'C': // C键清空曲线
			curve = Bhyin::BazierCurve();
			break;
		case 'a':
		case 'A': // A键修改动画状态
			enable_animation = !enable_animation;
			break;
		}
		});

	// 使用utils里的reshape回调函数
	glutReshapeFunc(DefaultReshape);
	
	glutMainLoop();
	return 0;
}


void DrawBazierCurve(float ratio)
{

	// 线条宽度和点大小
	glLineWidth(4);
	glPointSize(16);

	/*使用glVertexPointer和glDrawArrays绘制顶点，而不采用glBegin和glEnd*/
	// 控制点
	glColor3fv(GetColorPtr(0)); // 颜色
	VertexPointer(3, GL_DOUBLE, 0, curve.GetHandlesData()); // 指定数据
	DrawArrays(GL_POINTS, 0, curve.GetHandlesNum()); // 绘制控制点
	DrawArrays(GL_LINE_STRIP, 0, curve.GetHandlesNum()); // 绘制控制点间的连线

	int frame = int(ratio * curve.GetSamplesNum()); // 获取帧序号

	glLineWidth(2);
	glPointSize(8);
	// 其它控制点
	for (int i = 1; i < curve.GetHandlesNum() - 1; ++i)
	{
		glColor3fv(GetColorPtr(i)); // 颜色
		VertexPointer(3, GL_DOUBLE, 0, curve.GetFrameData(frame, i)); // 数据
		DrawArrays(GL_POINTS, 0, curve.GetHandlesNum() - i); // 控制点
		DrawArrays(GL_LINE_STRIP, 0, curve.GetHandlesNum() - i); // 控制点连线
	}

	glLineWidth(6);
	glPointSize(20);
	// 贝塞尔曲线
	glColor3f(1.0, 1.0, 1.0); // 颜色
	VertexPointer(3, GL_DOUBLE, 3 * curve.GetFrameSize() * sizeof(double), curve.GetCurveData());  // 数据
	DrawArrays(GL_POINTS, frame - 1, 1); // 当前帧的点，即当前贝塞尔曲线的终点
	DrawArrays(GL_LINE_STRIP, 0, frame); // 已有的所有帧上的点的连线，即已经绘制出的贝塞尔曲线


	/*//-------下面的代码，使用glBegin和glEnd，和上面是 基本 等效的（大概）-------
	glColor3fv(GetColorPtr(0)); // 颜色
	double* dptr = curve.GetHandlesData();
	glBegin(GL_POINTS);
	for (int i = 0; i < curve.GetHandlesNum(); ++i)glVertex3dv(dptr + 3 * i);
	glEnd();
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < curve.GetHandlesNum(); ++i)glVertex3dv(dptr + 3 * i);
	glEnd();

	int frame = int(ratio * curve.GetSamplesNum()); // 获取帧序号

	for (int i = 1; i < curve.GetHandlesNum() - 1; ++i)
	{
		glColor3fv(GetColorPtr(i)); // 颜色
		dptr = curve.GetFrameData(frame, i);
		glBegin(GL_POINTS);
		for (int j = 0; j < curve.GetHandlesNum() - i; ++j)glVertex3dv(dptr + 3 * j);
		glEnd();
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < curve.GetHandlesNum() - i; ++j)glVertex3dv(dptr + 3 * j);
		glEnd();
	}

	glColor3fv(GetColorPtr(curve.GetHandlesNum() - 1)); // 颜色
	dptr = curve.GetCurveData();
	int strip = 3 * curve.GetFrameSize();
	glBegin(GL_POINTS);
	glVertex3dv(dptr + frame * strip);
	glEnd();
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < frame; ++i)glVertex3dv(dptr + i * strip);
	glEnd();
	*/
}


void DisplayTips()
{
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(-1.0, 0.9);
	for (char c : AddPoint)glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	glRasterPos2f(-1.0, 0.8);
	for (char c : ChangeState)glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	for (char c : ClearScreen)glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	glRasterPos2f(-1.0, 0.7);
	if (enable_animation)for (char c : EnableState)glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	else for (char c : DisableState)glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	glRasterPos2f(-1.0, 0.6);
	for (char c : KeyState)glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, KeyChar);
}

