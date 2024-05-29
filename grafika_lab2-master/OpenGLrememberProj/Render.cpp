#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"
#include "math.h"

bool textureMode = true;
bool lightMode = true;
double* Normal(double* a, double* b, double* c)
{
	double ABx = b[0] - a[0];
	double ABy = b[1] - a[1];
	double ABz = b[2] - a[2];
	double ACx = c[0] - a[0];
	double ACy =c[1] - a[1];
	double ACz = c[2] - a[2];
	double aa[] = { ABx,ABy,ABz };
	double bb[] = { ACx,ACy,ACz };
	double nx = aa[1] * bb[2] - bb[1] * aa[2];
	double ny = -aa[0] * bb[2] + bb[0] * aa[2];
	double nz = aa[0] * bb[1] - bb[0] * aa[1];
	double n = sqrt(nx * nx + ny * ny + nz * nz);
	nx /= n;
	ny /= n;
	nz /= n;
	double nn[] = { nx,ny,nz };
	return nn;
}
//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


void Circle(double* c, double r) {
	double d = 0;
	while (d <= 180) {
		glVertex3dv(c);
		glVertex3d(r * cos(d) + c[0], r * sin(d) + c[1], c[2]);
		glVertex3d(r * cos(d + 0.1) + c[0], r * sin(d + 0.1) + c[1], c[2]);
		d += 0.1;
	}
}
void Round(double* a, double* b, double h) {
	glColor3d(0.3, 0.5, 0.2);
	glBegin(GL_TRIANGLES);
	double middle[] = { (a[0] + b[0]) / 2, (a[1] + b[1]) / 2, (a[2] + b[2]) / 2 };
	double radius = sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2])) / 2;
	Circle(middle, radius);
	double middle1[] = { middle[0], middle[1], middle[2] + h };
	Circle(middle1, radius);
	glEnd();
	glColor3d(0.2, 0.2, 0.5);
	glBegin(GL_QUADS);
	double d = 0;
	while (d <= 180) {
		double Z[] = { radius * cos(d) + middle[0], radius * sin(d) + middle[1], middle[2] };
		double X[] = { radius * cos(d) + middle1[0], radius * sin(d) + middle1[1], middle1[2] };
		double C[] = { radius * cos(d + 0.1) + middle1[0], radius * sin(d + 0.1) + middle1[1], middle1[2] };
		glNormal3dv(Normal(C,X, Z));
		glVertex3d(radius * cos(d) + middle[0], radius * sin(d) + middle[1], middle[2]);
		glVertex3d(radius * cos(d) + middle1[0], radius * sin(d) + middle1[1], middle1[2]);
		glVertex3d(radius * cos(d + 0.1) + middle1[0], radius * sin(d + 0.1) + middle1[1], middle1[2]);
		glVertex3d(radius * cos(d + 0.1) + middle[0], radius * sin(d + 0.1) + middle[1], middle[2]);
		d += 0.1;
	}
	glEnd();
	glColor3d(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3dv(a);
	glVertex3dv(b);
	glEnd();
}


void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  
	
		double O1[] = { 0, 0, 1 };
		double A1[] = { -8, 2, 1 };
		double B1[] = { -5,7,1 };
		double C1[] = { -14,10,1 };
		double D1[] = { -15,2, 1 };
		double E1[] = { -10,1,1 };
		double F1[] = { -9,-5,1 };
		double G1[] = { -5,-6,1 };
		double A2[] = { -8, 2, 9 };
		double B2[] = { -5,7,9 };
		double C2[] = { -14,10,9 };
		double D2[] = { -15,2, 9 };
		double E2[] = { -10,1,9 };
		double F2[] = { -9,-5,9 };
		double G2[] = { -5,-6,9 };
		double oo2[] = { 0, 0, 9 };
		double oo1[] = { 0, 0, 1 };
		/*	glBegin(GL_LINES);
			glColor3d(0.3, 0.5, 0.2);
			glVertex3d(0, 0, 0);
			glVertex3d(8,2,1);
			glVertex3d(0, 0, 0);
			glVertex3d(3, 9, 1);
			glEnd();*/
			//glBegin(GL_TRIANGLES);
			//Round(O1, A1, 8);
			//glEnd();
		glBegin(GL_TRIANGLES);
		glNormal3d(0, 0, 1);
		glColor3d(0.3, 0.5, 0.2);
		/*double nx1 = 0 * A1[3] - A1[2] * 1;
			double ny1 = -0 * A1[3] + A1[1] * 1;
			double nz1 = 0 * A1[2] - A1[1] * 0;
			double nd1 = sqrt(nx1*nx1+ny1*ny1+nz1*nz1);
			nx1 /= nd1;
			ny1 /= nd1;
			nz1 /= nd1;
			double n1[] = { nx1,ny1,nz1 };*/
			//glNormal3dv(n1);
		glVertex3dv(D1);
		glVertex3dv(C1);
		glVertex3dv(B1);

		glVertex3dv(D1);
		glVertex3dv(B1);
		glVertex3dv(A1);

		glVertex3dv(D1);
		glVertex3dv(E1);
		glVertex3d(-10, 2, 1);

		glVertex3dv(E1);
		glVertex3dv(A1);
		glVertex3dv(G1);

		glVertex3dv(E1);
		glVertex3dv(F1);
		glVertex3dv(G1);

		glVertex3dv(F1);
		glVertex3dv(G1);
		glVertex3dv(oo1);

		glVertex3dv(E1);
		glVertex3d(-4, 1, 1);
		glVertex3dv(G1);

		glVertex3dv(E1);
		glVertex3dv(A1);
		glVertex3dv(oo1);

		glVertex3dv(E1);
		glVertex3dv(G1);
		glVertex3dv(oo1);

		glVertex3dv(E1);
		glVertex3dv(A1);
		glVertex3d(-10, 2, 1);

		//glVertex3dv(A1);
		//glVertex3dv(B1);
		//glVertex3dv(D1);

		//glVertex3dv(D1);
		//glVertex3dv(C1);
		//glVertex3d(5,0,1);

		//glVertex3dv(C1);
		//glVertex3dv(B1);
		//glVertex3dv(A1);

		glEnd();

		glBegin(GL_TRIANGLES);
		glNormal3d(0, 0, 1);
		glColor3d(0.3, 0.5, 0.2);
		/*double nx1 = 0 * A1[3] - A1[2] * 1;
			double ny1 = -0 * A1[3] + A1[1] * 1;
			double nz1 = 0 * A1[2] - A1[1] * 0;
			double nd1 = sqrt(nx1*nx1+ny1*ny1+nz1*nz1);
			nx1 /= nd1;
			ny1 /= nd1;
			nz1 /= nd1;
			double n1[] = { nx1,ny1,nz1 };*/
			//glNormal3dv(n1);
		glVertex3dv(D2);
		glVertex3dv(C2);
		glVertex3dv(B2);

		glVertex3dv(D2);
		glVertex3dv(B2);
		glVertex3dv(A2);

		glVertex3dv(D2);
		glVertex3dv(E2);
		glVertex3d(-10, 2, 9);

		glVertex3dv(E2);
		glVertex3dv(A2);
		glVertex3dv(G2);

		glVertex3dv(E2);
		glVertex3dv(F2);
		glVertex3dv(G2);

		glVertex3dv(F2);
		glVertex3dv(G2);
		glVertex3dv(oo2);

		glVertex3dv(E2);
		glVertex3d(-4, 1, 9);
		glVertex3dv(G2);

		glVertex3dv(E2);
		glVertex3dv(A2);
		glVertex3dv(oo2);

		glVertex3dv(E2);
		glVertex3dv(G2);
		glVertex3dv(oo2);

		glVertex3dv(E2);
		glVertex3dv(A2);
		glVertex3d(-10, 2, 9);
		glEnd();
		glBegin(GL_QUADS);

		glColor3d(0.2, 0.2, 0.5);
		glNormal3dv(Normal(oo2, oo1, A2));
		glVertex3dv(oo2);
		glVertex3dv(oo1);
		glVertex3dv(A1);
		glVertex3dv(A2);
		glEnd();
		glBegin(GL_QUADS);
			glNormal3dv(Normal(A2, A1, B2));
		glVertex3dv(A2);
		glVertex3dv(A1);
		glVertex3dv(B1);
		glVertex3dv(B2);
		//glVertex3d(0,0,9);
			//glVertex3d(0,0,1);
		glEnd();
		glBegin(GL_QUADS);
		glNormal3dv(Normal(B2, B1, C2));
		glVertex3dv(B2);
		glVertex3dv(B1);
		glVertex3dv(C1);
		glVertex3dv(C2);
		glEnd();
	/*	glBegin(GL_QUADS);
		glNormal3dv(Normal(C2, C1, D2));
		glVertex3dv(C2);
		glVertex3dv(C1);
		glVertex3dv(D1);
		glVertex3dv(D2);
		glEnd();*/
		glBegin(GL_QUADS);
		glNormal3dv(Normal(D2, D1, E2));
		glVertex3dv(D2);
		glVertex3dv(D1);
		glVertex3dv(E1);
		glVertex3dv(E2);
		glEnd();
		glBegin(GL_QUADS);
		glNormal3dv(Normal(E2, E1, F2));
		glVertex3dv(E2);
		glVertex3dv(E1);
		glVertex3dv(F1);
		glVertex3dv(F2);
		glEnd();

		glBegin(GL_QUADS);
		glNormal3dv(Normal(F2, F1, G2));
		glVertex3dv(F2);
		glVertex3dv(F1);
		glVertex3dv(G1);
		glVertex3dv(G2);
		glEnd();
		glBegin(GL_QUADS);
		glNormal3dv(Normal(G2, G1, oo2));
		glVertex3dv(G2);
		glVertex3dv(G1);
		glVertex3dv(oo1);
		glVertex3dv(oo2);
		glEnd();
		double x0 = -14;
		double y0 = 10;

		double x01 = -14;// -14 * cos(alfa) + 7 * sin(alfa);
		double y01 = 10;//14 * sin(alfa) + 7 * cos(alfa);
		for (double i = 0.5628; i <= 1.5628; i += 0.0001)
		{

			double x = -14.5 + 4 * cos(i * 3.141593);
			double y = 6 + 4 * sin(i * 3.141593);

			double x1 = -14.5 + 4 * cos(i * 3.141593);//(-14 * cos(alfa) + 7 * sin(alfa) + (-13 * cos(alfa) + 2 * sin(alfa))) / 2 + 2.549509 * cos(i * 3.141593 - alfa);
			double y1 = 6 + 4 * sin(i * 3.141593);//(14 * sin(alfa) + 7 * cos(alfa) + (13 * sin(alfa) + 2 * cos(alfa))) / 2 + 2.549509 * sin(i * 3.141593 - alfa);

			glBegin(GL_POLYGON); glColor3d(0.3, 0.5, 0.2); // glColor3d(0, 0, 0);
			double Z[] = { x0, y0, 1 };
			double X[] = { x, y, 1 };
			glNormal3dv(Normal(B1, Z, X));
			glVertex3dv(B1);
			glVertex3d(x0, y0, 1);
			glVertex3d(x, y, 1);
			glEnd();

			glBegin(GL_POLYGON);
			double Z2[] = {x01, y01, 1};
			double X2[] = { x1, y1, 1 };
			glNormal3dv(Normal(B2, Z2, X2));
			glVertex3dv(B2);
			glVertex3d(x01, y01, 9);
			glVertex3d(x1, y1, 9);
			glEnd();

			glBegin(GL_POLYGON); glColor3d(0.2, 0.2, 0.5);//glColor3d(0.2, 0.25, 0);
			glVertex3d(x0, y0, 1);
			glVertex3d(x, y, 1);
			glVertex3d(x1, y1, 9);
			glVertex3d(x01, y01, 9);
			glEnd();

			x0 = x;
			y0 = y;

			x01 = x1;
			y01 = y1;

		}
	/*glBegin(GL_TRIANGLES);
	glColor3d(0.3, 0.5, 0.2);
	double i = 0;
	while (i < 360)
	{
		glVertex3d(4,4,1);
		glVertex3d(cos(i), sin(i), 1);
		glVertex3dv(D1);
		i += 1;
	}
	glEnd();*/
	
	/*//������ ��������� ���������� ��������
	double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();
	//����� ��������� ���������� ��������*/


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
void Render(double delta_time)
{

}

