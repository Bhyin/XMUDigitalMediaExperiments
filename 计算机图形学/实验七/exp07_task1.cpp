#define _CRT_SECURE_NO_WARNINGS 1

#include "GL/glut.h"
#include "Bazier.h"
#include "utils.h" // ʵ������д��utils
#include <cmath>

// ���ߵĲ���������
const int SampleNum = 200;

// ��ɫ���������ֲ�ͬ��εĿ��Ƶ�
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
// ��ȡ��ɫ�����ָ��
inline float* GetColorPtr(int i) { return colors + 3 * ((i + 14) % 14); }

// ����������ʾ
const char AddPoint[] = "Click left mouse button to append point.";
const char ChangeState[] = "Press 'A': enable/disable animation.";
const char ClearScreen[] = "Press 'C': clear screen.";
const char EnableState[] = "Animation State: Enable.";
const char DisableState[] = "Animation State: Disable";
const char KeyState[] = "Press Key: ";
char KeyChar = '-';

// ���ڶ����Ĳ���
float frame_ratio = 0.5f;
bool enable_animation = true;

// ���߶���
/*
��ʼû���κ����ߣ���������ڿ��Դ������Ƶ㣬���¼����ϵ� ��c�� �����������Ļ
*/
Bhyin::BazierCurve curve = Bhyin::BazierCurve();


// ����Ļ���Ͻ���ʾ�ַ���
void DisplayTips();

// ���Ʊ���������
void DrawBazierCurve(float ratio);

int main(int argc, char** argv)
{
	Init(argc, argv);
	Window("exp07_task1", 800, 800);

	glShadeModel(GL_SMOOTH);
	
	glutDisplayFunc([]() {
		glClear(GL_COLOR_BUFFER_BIT);

		if(curve.exist)DrawBazierCurve(frame_ratio); // ��������
		DisplayTips(); // ��������

		glFlush();
		glutSwapBuffers();
		});

	// ����ƶ�
	glutMotionFunc([](int x, int y) {
		//frame_ratio = 0.0;
		double posX, posY;
		WindowToOpenGL(x, y, &posX, &posY);

		// ����е㱻�϶�����ʵʱ�޸���λ��
		if (curve.isDragging)
		{
			curve.Drag(posX, posY);
			curve.RefreshCurve();
		}
		});

	// �����
	glutMouseFunc([](int button, int state, int x, int y) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) // �������
		{
			KeyChar = '0';
			/*frame_ratio = 0.0;*/
			double posX, posY;
			WindowToOpenGL(x, y, &posX, &posY);
			//printf("%d, %d, %f, %f \n",x, y, posX, posY);
			
			// ���ĳһ�����Ƿ����ѡ��
			curve.isHandleDragged(posX, posY);
			glutPostRedisplay(); // �ػ洰��
		}
		else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) // ����ɿ�
		{
			double posX, posY;
			WindowToOpenGL(x, y, &posX, &posY);

			// ���ĳ�����㱻�϶������޸Ķ����λ��,�������϶�״̬
			if (curve.isDragging)
			{
				curve.Drag(posX, posY);
				curve.isDragging = false;
				curve.dragIndex = -1;
				curve.RefreshCurve();
			}
			else
			{
				// ûѡ�У��Ǿ��ڴ˴���ӿ��Ƶ�
				curve.AppendHandle(posX, posY, 0.0);
			}
		}
		});

	// ���е���
	glutIdleFunc([]() {
		if (enable_animation) // ������ö���
		{
			frame_ratio += 1 / 240.0; // �������̫�죬���԰ѷ�ĸ�Ĵ�һ��
			if (frame_ratio >= 1.0)frame_ratio = 0.0;
		}
		else frame_ratio = 1.0; // ��ʾ��������
		
		glutPostRedisplay();
		});

	// ��������
	glutKeyboardFunc([](unsigned char key, int x, int y) {
		KeyChar = key;
		switch (key) {
		case 27:
			exit(0);
		case 'c':
		case 'C': // C���������
			curve = Bhyin::BazierCurve();
			break;
		case 'a':
		case 'A': // A���޸Ķ���״̬
			enable_animation = !enable_animation;
			break;
		}
		});

	// ʹ��utils���reshape�ص�����
	glutReshapeFunc(DefaultReshape);
	
	glutMainLoop();
	return 0;
}


void DrawBazierCurve(float ratio)
{

	// ������Ⱥ͵��С
	glLineWidth(4);
	glPointSize(16);

	/*ʹ��glVertexPointer��glDrawArrays���ƶ��㣬��������glBegin��glEnd*/
	// ���Ƶ�
	glColor3fv(GetColorPtr(0)); // ��ɫ
	VertexPointer(3, GL_DOUBLE, 0, curve.GetHandlesData()); // ָ������
	DrawArrays(GL_POINTS, 0, curve.GetHandlesNum()); // ���ƿ��Ƶ�
	DrawArrays(GL_LINE_STRIP, 0, curve.GetHandlesNum()); // ���ƿ��Ƶ�������

	int frame = int(ratio * curve.GetSamplesNum()); // ��ȡ֡���

	glLineWidth(2);
	glPointSize(8);
	// �������Ƶ�
	for (int i = 1; i < curve.GetHandlesNum() - 1; ++i)
	{
		glColor3fv(GetColorPtr(i)); // ��ɫ
		VertexPointer(3, GL_DOUBLE, 0, curve.GetFrameData(frame, i)); // ����
		DrawArrays(GL_POINTS, 0, curve.GetHandlesNum() - i); // ���Ƶ�
		DrawArrays(GL_LINE_STRIP, 0, curve.GetHandlesNum() - i); // ���Ƶ�����
	}

	glLineWidth(6);
	glPointSize(20);
	// ����������
	glColor3f(1.0, 1.0, 1.0); // ��ɫ
	VertexPointer(3, GL_DOUBLE, 3 * curve.GetFrameSize() * sizeof(double), curve.GetCurveData());  // ����
	DrawArrays(GL_POINTS, frame - 1, 1); // ��ǰ֡�ĵ㣬����ǰ���������ߵ��յ�
	DrawArrays(GL_LINE_STRIP, 0, frame); // ���е�����֡�ϵĵ�����ߣ����Ѿ����Ƴ��ı���������


	/*//-------����Ĵ��룬ʹ��glBegin��glEnd���������� ���� ��Ч�ģ���ţ�-------
	glColor3fv(GetColorPtr(0)); // ��ɫ
	double* dptr = curve.GetHandlesData();
	glBegin(GL_POINTS);
	for (int i = 0; i < curve.GetHandlesNum(); ++i)glVertex3dv(dptr + 3 * i);
	glEnd();
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < curve.GetHandlesNum(); ++i)glVertex3dv(dptr + 3 * i);
	glEnd();

	int frame = int(ratio * curve.GetSamplesNum()); // ��ȡ֡���

	for (int i = 1; i < curve.GetHandlesNum() - 1; ++i)
	{
		glColor3fv(GetColorPtr(i)); // ��ɫ
		dptr = curve.GetFrameData(frame, i);
		glBegin(GL_POINTS);
		for (int j = 0; j < curve.GetHandlesNum() - i; ++j)glVertex3dv(dptr + 3 * j);
		glEnd();
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < curve.GetHandlesNum() - i; ++j)glVertex3dv(dptr + 3 * j);
		glEnd();
	}

	glColor3fv(GetColorPtr(curve.GetHandlesNum() - 1)); // ��ɫ
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

