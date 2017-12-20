#include "Render.h"

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"



bool textureMode = true;
bool lightMode = true; 

bool alpha = false;

int ugol = 0;
GLuint texId[2];

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;


	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
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
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


			 //Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}


	//рисует сферу и линии под источником света, вызывается движком
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
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
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

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




		  //старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}


	//двигаем свет по плоскости, в точку где мышь
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

	if (key == 'A' && !alpha)
	{
		lightMode = !lightMode;
		textureMode = !textureMode;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		alpha = true;
	}

	else if (key == 'A' && alpha)
	{
		lightMode = !lightMode;
		textureMode = !textureMode;

		glDisable(GL_BLEND);
		alpha = false;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}




//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);


	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;
	RGBTRIPLE *texarray1; //я

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);





	glGenTextures(2, texId);

	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId[0]);



	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	
	//отчистка памяти
	free(texCharArray);
	free(texarray);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	char *texCharArray1; //я

	OpenGL::LoadBMP("texture1.bmp", &texW, &texH, &texarray1); //я
	OpenGL::RGBtoChar(texarray1, texW, texH, &texCharArray1); //я

	glBindTexture(GL_TEXTURE_2D, texId[1]); //я

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray1); //я

	free(texCharArray1); //я
	free(texarray1); //я
	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH);


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
}




/*void normal(double a1[], double b1[], double c1[], double vn[])
{
	double a[] = { a1[0] - b1[0],a1[1] - b1[1],a1[2] - b1[2] };
	double b[] = { c1[0] - b1[0],c1[1] - b1[1],c1[2] - b1[2] };

	vn[0] = a[1] * b[2] - b[1] * a[2];
	vn[1] = -a[0] * b[2] + b[0] * a[2];
	vn[2] = a[0] * b[1] - b[0] * a[1];

	double length = sqrt(pow(vn[0], 2) + pow(vn[1], 2) + pow(vn[2], 2));

	vn[0] /= length;
	vn[1] /= length;
	vn[2] /= length;
}*/

/*void fun_low_vip() //выпуклость на нижнем основании
{
	double a[] = { -5,-2,-5 };
	double h[] = { 7,-2,-5 };
	double m[] = { 1,-4,-5 };
	double o[] = { 1, 6,-5 }; //центр окружности
	int r = 10; //радиус окружности
	double point[] = { 0,0,-5 }; //точка дуги, x и y выбираем в цикле
	int alpha = 180 + asin(0.8) * 180 / 3.14;
	int betta = 360 - asin(0.8) * 180 / 3.14;
	glBegin(GL_POLYGON);

	double vn[] = { 0,0,0 };//вектор нормали
	normal(a, m, h, vn);
	glNormal3dv(vn);

	glTexCoord2d(420.0 / 512.0, (512.0 - 160.0) / 512.0);
	glVertex3dv(a);
	for (int i = alpha; i <= betta; i++)
	{
		point[0] = o[0] + r*cos(i*3.14 / 180);
		point[1] = o[1] + r*sin(i*3.14 / 180);
		glTexCoord2d((360.0 - 100.0 * cos(i*3.14 / 180.0)) / 512.0, (512.0 - 80.0 + 100.0*sin(i*3.14 / 180)) / 512.0);
		glVertex3dv(point);
	}
	glTexCoord2d(300.0 / 512.0, (512.0 - 160.0) / 512.0);
	glVertex3dv(h);
	glEnd();
}*/

/*void fun_low() //нижнее основание
{
	double a[] = { -5,-2,-5 };
	double b[] = { -5, 1,-5 };
	double c[] = { -1, 1,-5 };
	double d[] = { -2, 4,-5 };
	double e[] = { 7, 4,-5 };
	double f[] = { 5, 1,-5 };
	double g[] = { 7, 1,-5 };
	double h[] = { 7,-2,-5 };
	//для вогнутости
	double n[] = { 2,2,-5 };
	double o[] = { 2.5, 8,-5 }; //центр окружности
	float r = 6.02; //радиус окружности
	double point[] = { 0,0,-5 }; //точка дуги, x и y выбираем в цикле
	int alpha = 180 + asin(4 / 6.02) * 180 / 3.14;
	int betta = 360 - asin(4 / 6.02) * 180 / 3.14;

	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый

	double vn[] = { 0,0,0 };//вектор нормали
	normal(g, b, a, vn);
	glNormal3dv(vn);


	glBegin(GL_QUADS);
	glTexCoord2d(420.0/512.0, (512.0-160.0)/512.0);
	glVertex3dv(a);
	glTexCoord2d(420.0 / 512.0, (512.0 - 130.0) / 512.0);
	glVertex3dv(b);
	glTexCoord2d(300.0 / 512.0, (512.0 - 130.0) / 512.0);
	glVertex3dv(g);
	glTexCoord2d(300.0 / 512.0, (512.0 - 160.0) / 512.0);
	glVertex3dv(h);
	glEnd();

	glBegin(GL_QUAD_STRIP);
		glTexCoord2d(390.0 / 512.0, 1.0 - 100.0 / 512.0);
		glVertex3dv(d);
		glTexCoord2d(380.0 / 512.0, 1.0 - 130.0 / 512.0);
		glVertex3dv(c);
		double osn[] = { -1,1,-5 };
		int alala = 0;
		for (int i = alpha; i < betta; i++)
		{
			alala++;
			point[0] = o[0] + r*cos(i*3.14 / 180);
			point[1] = o[1] + r*sin(i*3.14 / 180);
			glTexCoord2d((345.0 - 60.2*cos(i*3.14 / 180.0)) / 512.0, (512.0 - 60.0 + 60.2*sin(i*3.14 / 180.0)) / 512.0);
			glVertex3dv(point);
			double osn[] = { -1 + 0.06186*alala,1,-5 };
			glTexCoord2d((380.0 - (60.0 / 97.0)*alala) / 512.0, 1.0 - 130.0 / 512.0); //исправь
			glVertex3dv(osn);
		}
		glTexCoord2d(300.0 / 512.0, 1.0 - 100.0 / 512.0);
		glVertex3dv(e);
		glTexCoord2d(320.0 / 512.0, 1.0 - 130.0 / 512.0);
		glVertex3dv(f);
	glEnd();

	fun_low_vip(); //выпуклость на нижнем основании
}*/




/*void fun_up_vip() //выпуклость на верхнем основании
{
	double a1[] = { -5,-2,5 };
	double h1[] = { 7,-2, 5 };
	double m1[] = { 1,-4, 5 };
	double o1[] = { 1, 6, 5 }; //центр окружности
	int r = 10; //радиус окружности
	double point1[] = { 0,0,5 }; //точка дуги, x и y выбираем в цикле
	int alpha = 180 + asin(0.8) * 180 / 3.14;
	int betta = 360 - asin(0.8) * 180 / 3.14;
	glBegin(GL_POLYGON);

	double vn[] = { 0,0,0 };//вектор нормали
	normal(h1, m1, a1, vn);
	glNormal3dv(vn);

	glTexCoord2d(100.0 /512.0, (512.0 -160.0)/512.0);
	glVertex3dv(a1);
	for (int i = alpha; i <= betta; i++)
	{
		point1[0] = o1[0] + r*cos(i*3.14 / 180);
		point1[1] = o1[1] + r*sin(i*3.14 / 180);
		glTexCoord2d((160.0 + 100.0 * cos(i*3.14 / 180.0))/512.0,(512.0 - 80.0 + 100.0*sin(i*3.14 / 180))/512.0);
		glVertex3dv(point1);
	}
	glTexCoord2d(220.0 / 512.0, (512.0 - 160.0) / 512.0);
	glVertex3dv(h1);
	glEnd();
}*/

/*void fun_up() //верхнее основание
{
	double a1[] = { -5,-2, 5 };
	double b1[] = { -5, 1, 5 };
	double c1[] = { -1, 1, 5 };
	double d1[] = { -2, 4, 5 };
	double e1[] = { 7, 4, 5 };
	double f1[] = { 5, 1, 5 };
	double g1[] = { 7, 1, 5 };
	double h1[] = { 7,-2, 5 };//для вогнутости
	double n[] = { 2,2,5 };
	double o[] = { 2.5, 8,5 }; //центр окружности
	float r = 6.02; //радиус окружности
	double point[] = { 0,0,5 }; //точка дуги, x и y выбираем в цикле
	int alpha = 180 + asin(4 / 6.02) * 180 / 3.14;
	int betta = 360 - asin(4 / 6.02) * 180 / 3.14;

	//glColor3f(1.0f, 1.0f, 0.0f); //желтый
	
	//glColor4f(1.0f, 1.0f, 0.0f, 0.0f);

	double vn[] = { 0,0,0 };//вектор нормали
	normal(a1, b1, g1, vn);
	glNormal3dv(vn);
	//glNormal3f(0,0,1);

	glBegin(GL_QUADS); //A1B1G1H1
	//переделать, вместо z1 и z2 посчитать самому
	GLdouble z1 = 100.0 / 512.0;
	GLdouble z2 = (512.0 - 160.0) / 512.0;
		glTexCoord2d(z1, z2);
		glVertex3dv(a1);
		z1 = 100.0 / 512.0;
		z2 = (512.0 - 130.0) / 512.0;
		glTexCoord2d(z1, z2);
		glVertex3dv(b1);
		z1 = 220.0 / 512.0;
		z2 = (512.0 - 130.0) / 512.0;
		glTexCoord2d(z1, z2);
		glVertex3dv(g1);
		z1 = 220.0 / 512.0;
		z2 = (512.0 - 160.0) / 512.0;
		glTexCoord2d(z1, z2);
		glVertex3dv(h1);
	glEnd();

	glBegin(GL_QUAD_STRIP); //C1D1E1F1
		glTexCoord2d(130.0 / 512.0, 1.0 - 100.0 / 512.0);
		glVertex3dv(d1);
		glTexCoord2d(140.0/512.0, 1.0 - 130.0/512.0);
		glVertex3dv(c1);
		double osn[] = { -1,1,5 };
		int alala = 0;
		for (int i = alpha; i < betta; i++)
		{
			alala++;
			point[0] = o[0] + r*cos(i*3.14 / 180);
			point[1] = o[1] + r*sin(i*3.14 / 180);
			glTexCoord2d((175.0 + 60.2*cos(i*3.14 / 180.0))/512.0 , (512.0 - 60.0 + 60.2*sin(i*3.14 / 180.0)) / 512.0);
			glVertex3dv(point);
			double osn[] = { -1 + 0.06186*alala,1,5 };
			glTexCoord2d((140.0 + (60.0/97.0)*alala)/512.0, 1.0 - 130.0 / 512.0); //исправь
			glVertex3dv(osn);
		}
		glTexCoord2d(220.0 / 512.0, 1.0 - 100.0 / 512.0);
		glVertex3dv(e1);
		glTexCoord2d(200.0 / 512.0, 1.0 - 130.0 / 512.0);
		glVertex3dv(f1);
	glEnd();

	fun_up_vip(); //выпуклость на верхнем основании
}*/

/*void fun_side() //боковая сторона
{
	//проверить все нормали, правильно ли точки закидываю?
	
	//точки нижнего основания
	double a[] = { -5,-2,-5 };
	double b[] = { -5, 1,-5 };
	double c[] = { -1, 1,-5 };
	double d[] = { -2, 4,-5 };
	double e[] = { 7, 4,-5 };
	double f[] = { 5, 1,-5 };
	double g[] = { 7, 1,-5 };
	double h[] = { 7,-2,-5 };
	//точки верхнего основания
	double a1[] = { -5,-2, 5 };
	double b1[] = { -5, 1, 5 };
	double c1[] = { -1, 1, 5 };
	double d1[] = { -2, 4, 5 };
	double e1[] = { 7, 4, 5 };
	double f1[] = { 5, 1, 5 };
	double g1[] = { 7, 1, 5 };
	double h1[] = { 7,-2, 5 };
	//точки для дуги выпуклости
	double o11[] = { 1, 6,-5 }; //центр окружности
	double o12[] = { 1, 6, 5 }; //центр окружности
	int r1 = 10; //радиус окружности
	double point11[] = { 0,0,-5 }; //точка дуги, x и y выбираем в цикле
	double point12[] = { 0,0,5 }; //точка дуги, x и y выбираем в цикле
	int alpha1 = 180 + asin(0.8) * 180 / 3.14;
	int betta1 = 360 - asin(0.8) * 180 / 3.14;
	//соединяем точки верхнего и нижнего основания

	double vn[] = { 0,0,0 };//вектор нормали

	
	glBegin(GL_QUAD_STRIP);
	//glColor3f(1.0f, 1.0f, 0.0f); //желтый

	//normal(a1, a, b, vn);
	//glNormal3dv(vn);

	//glVertex3dv(a);
	//glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	//glVertex3dv(a1);
	//glColor3f(1.0f, 1.0f, 0.0f); //желтый

	normal(c1, b1, b, vn);
	glNormal3dv(vn);

	glColor3f(1.0f, 1.0f, 0.0f); //желтый
	glTexCoord2d(467.0/512.0, (512.0 - 300.0)/512.0);
	glVertex3dv(b);
	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	glTexCoord2d(467.0 / 512.0, (512.0 - 200.0) / 512.0);
	glVertex3dv(b1);
	glColor3f(1.0f, 1.0f, 0.0f); //желтый

	normal(d1, c1, c, vn);
	glNormal3dv(vn);
	glTexCoord2d(427.0 / 512.0, (512.0 - 300.0) / 512.0);
	glVertex3dv(c);
	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	glTexCoord2d(427.0 / 512.0, (512.0 - 200.0) / 512.0);
	glVertex3dv(c1);
	glColor3f(1.0f, 1.0f, 0.0f); //желтый

	glTexCoord2d(395.0 / 512.0, (512.0 - 300.0) / 512.0);
	glVertex3dv(d);
	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	glTexCoord2d(395.0 / 512.0, (512.0 - 200.0) / 512.0);
	glVertex3dv(d1);

	//	glBegin(GL_QUAD_STRIP);
	//glColor3f(0.0f, 1.0f, 0.0f); //green
	//glVertex3dv(c1);
	//glVertex3dv(d1);
	double o22[] = { 2.5, 8,5 }; //центр окружности
	double o21[] = { 2.5, 8,-5 }; //центр окружности
	float r = 6.02; //радиус окружности
	double point[] = { 0,0,5 }; //точка дуги, x и y выбираем в цикле

	double point1[] = { 0,0,-5 }; //точка дуги, x и y выбираем в цикле
	int alpha = 180 + asin(4 / 6.02) * 180 / 3.14;
	int betta = 360 - asin(4 / 6.02) * 180 / 3.14;
	int alala = 0;
	
	double pred[] = { -2, 4, 5 };//сначала тут d1
	double pred1[] = { -2, 4, -5 };//сначала тут d

	for (int i = alpha; i < betta; i++) //вогнутость
	{
		alala++;
		point[0] = o21[0] + r*cos(i*3.14 / 180);
		point[1] = o21[1] + r*sin(i*3.14 / 180);
		point1[0] = o22[0] + r*cos(i*3.14 / 180);
		point1[1] = o22[1] + r*sin(i*3.14 / 180);

		normal(point, pred, pred1, vn);
		glNormal3dv(vn);

		glColor3f(1.0f, 1.0f, 0.0f); //желтый
		glTexCoord2d((395.0 -(102/97)*alala)/ 512.0, (512.0 - 300.0) / 512.0);
		glVertex3dv(point1);
		glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
		glTexCoord2d((395.0 - (102 / 97)*alala) / 512.0, (512.0 - 200.0) / 512.0);
		glVertex3dv(point);

		pred[0] = point[0];
		pred[1] = point[1];
		pred1[0] = point1[0];
		pred1[1] = point1[1];
	}
	/*glVertex3dv(e1);
	glVertex3dv(f1);
	glEnd();


	normal(f1, e1, e, vn);
	glNormal3dv(vn);

	glColor3f(1.0f, 1.0f, 0.0f); //желтый
	glTexCoord2d(293.0 / 512.0, (512.0 - 300.0) / 512.0);
	glVertex3dv(e);
	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	glTexCoord2d(293.0 / 512.0, (512.0 - 200.0) / 512.0);
	glVertex3dv(e1);

	normal(g1, f1, f, vn);
	glNormal3dv(vn);

	glColor3f(1.0f, 1.0f, 0.0f); //желтый
	glTexCoord2d(257.0 / 512.0, (512.0 - 300.0) / 512.0);
	glVertex3dv(f);
	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	glTexCoord2d(257.0 / 512.0, (512.0 - 200.0) / 512.0);
	glVertex3dv(f1);

	normal(h1, g1, g, vn);
	glNormal3dv(vn);

	
	glColor3f(1.0f, 1.0f, 0.0f); //желтый
	glTexCoord2d(237.0 / 512.0, (512.0 - 300.0) / 512.0);
	glVertex3dv(g);
	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	glTexCoord2d(237.0 / 512.0, (512.0 - 200.0) / 512.0);
	glVertex3dv(g1);

	glColor3f(1.0f, 1.0f, 0.0f); //желтый
	glTexCoord2d(207.0 / 512.0, (512.0 - 300.0) / 512.0);
	glVertex3dv(h);
	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	glTexCoord2d(207.0 / 512.0, (512.0 - 200.0) / 512.0);
	glVertex3dv(h1);
	glColor3f(1.0f, 1.0f, 0.0f); //желтый

	
	pred[0] = 7;//сначала тут h1
	pred[1] = -2;
	pred[2] = 5;
	pred1[0] = 7;//сначала тут h
	pred1[1] = -2;
	pred1[2] = -5;

	for (int i = betta1; i >= alpha1; i--) //выпуклость
	{
		point11[0] = o11[0] + r1*cos(i*3.14 / 180);
		point11[1] = o11[1] + r1*sin(i*3.14 / 180);
		point12[0] = o12[0] + r1*cos(i*3.14 / 180);
		point12[1] = o12[1] + r1*sin(i*3.14 / 180);

		normal(point12, pred, pred1, vn);
		glNormal3dv(vn);

		glTexCoord2d((207.0 - (127 / 73)*alala) / 512.0, (512.0 - 300.0) / 512.0);
		glVertex3dv(point11);
		glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
		glTexCoord2d((207.0 - (127 / 73)*alala) / 512.0, (512.0 - 200.0) / 512.0);
		glVertex3dv(point12);
		glColor3f(1.0f, 1.0f, 0.0f); //желтый

		pred[0] = point12[0];
		pred[1] = point12[1];
		pred1[0] = point11[0];
		pred1[1] = point11[1];
	}

	//glColor3f(1.0f, 1.0f, 0.0f); //желтый
	//glVertex3dv(a);
	//glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	//glVertex3dv(a1);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texId[1]);
	glBegin(GL_TRIANGLES);

		normal(b1, a1, a, vn);
		glNormal3dv(vn);

		glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
		glTexCoord2d(80.0 / 512.0, (512.0 - 200.0) / 512.0);
		glVertex3dv(a1);
		glColor3f(1.0f, 1.0f, 0.0f); //желтый
		glTexCoord2d(80.0 / 512.0, (512.0 - 300.0) / 512.0);
		glVertex3dv(a);
		glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
		glTexCoord2d(50.0 / 512.0, (512.0 - 200.0) / 512.0);
		glVertex3dv(b1);

		normal(a, b, b1, vn);
		glNormal3dv(vn);

		glColor3f(1.0f, 1.0f, 0.0f); //желтый
		glTexCoord2d(80.0 / 512.0, (512.0 - 300.0) / 512.0);
		glVertex3dv(a);
		glTexCoord2d(50.0 / 512.0, (512.0 - 300.0) / 512.0);
		glVertex3dv(b);
		glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
		glTexCoord2d(50.0 / 512.0, (512.0 - 200.0) / 512.0);
		glVertex3dv(b1);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId[0]);
}*/

/*void fun_circle(void) //круг внутри призмы
{
	double center[] = {2,0,0};
	GLfloat theta;
	GLfloat pi = acos(-1.0); //пи
	GLfloat radius = 1.0f; // радиус

	glBegin(GL_TRIANGLE_FAN);
		glColor3f(1.0f, 0.0f, 0.0f); //красный
		glNormal3f(0,0,1);

		glTexCoord2d(0.0625, 0.9375);
		glVertex3f(center[0], center[1], 0.0f);

		for (GLfloat a = 0.0f; a <= 360.0f; a ++) {
			theta = pi * a / 180.0f;

			GLdouble x = 0.0625 + 0.0625 * cos(theta);
			GLdouble y = 0.9375 + 0.0625 * sin(theta);

			glTexCoord2d(x, y);
			glVertex3f(radius * cos(theta) + center[0], radius * sin(theta) + center[1], 0.0f);
		}
	glEnd();
}*/
void normal(double a1[], double b1[], double c1[], double vn[])
{
	double a[] = { a1[0] - b1[0],a1[1] - b1[1],a1[2] - b1[2] };
	double b[] = { c1[0] - b1[0],c1[1] - b1[1],c1[2] - b1[2] };

	vn[0] = a[1] * b[2] - b[1] * a[2];
	vn[1] = -a[0] * b[2] + b[0] * a[2];
	vn[2] = a[0] * b[1] - b[0] * a[1];

	double length = sqrt(pow(vn[0], 2) + pow(vn[1], 2) + pow(vn[2], 2));

	vn[0] /= length;
	vn[1] /= length;
	vn[2] /= length;
}
void fun_low_vip() //выпуклость на нижнем основании
{
	double a2[] = { -7,3.69,0 };
	double h2[] = { -2,7.69,0 };
	double m2[] = { 1,3.69,0 };
	double o2[] = { -3, 3.99,0 }; //центр окружности
	int r = 4; //радиус окружности
	double point[] = { 0,0,0 }; //точка дуги, x и y выбираем в цикле
	int alpha = 0;
	int betta = 180;
	glBegin(GL_POLYGON);
	glColor3f(0.5f, 0.5f, 0.5f);
	double vn[] = { 0,0,0 };//вектор нормали
	normal(a2, m2, h2, vn);
	glNormal3dv(vn);

	//glTexCoord2d(420.0 / 512.0, (512.0 - 160.0) / 512.0);
	//glVertex3dv(a);
	for (int i = alpha; i <= betta; i++)
	{
		point[0] = o2[0] + r * cos(i*3.14 / 180);
		point[1] = o2[1] + r * sin(i*3.14 / 180);
		glTexCoord2d((286.5 + 38.0 * cos(i*3.14 / 180.0)) / 512.0, (512.0 - 78.0 + 38.0*sin(i*3.14 / 180)) / 512.0);
		glVertex3dv(point);
	}
	//glTexCoord2d(300.0 / 512.0, (512.0 - 160.0) / 512.0);
	//glVertex3dv(h);
	glEnd();
}

void fun_up_vip() //выпуклость на верхнем основании
{
	double a1[] = { -7,3.69,2 };
	double h1[] = { -2,7.69,2 };
	double m1[] = { 1,3.69,2 };
	double o1[] = { -3, 3.99,2 }; //центр окружности

	int r = 4; //радиус окружности
	double point1[] = { 0,0,2 }; //точка дуги, x и y выбираем в цикле
	int alpha = 0;
	int betta = 180;
	glBegin(GL_POLYGON);

	double vn[] = { 0,0,0 };//вектор нормали
	normal(h1, m1, a1, vn);
	glNormal3dv(vn);

	//glTexCoord2d(100.0 / 512.0, (512.0 - 160.0) / 512.0);
	//glVertex3dv(a1);
	for (int i = alpha; i <= betta; i++)
	{
		point1[0] = o1[0] + r * cos(i*3.14 / 180);
		point1[1] = o1[1] + r * sin(i*3.14 / 180);
		glTexCoord2d((286.5 + 38.0 * cos(i*3.14 / 180.0)) / 512.0, (512.0 - 78.0 + 38.0*sin(i*3.14 / 180)) / 512.0);
		glVertex3dv(point1);
	}
	//glTexCoord2d(220.0 / 512.0, (512.0 - 160.0) / 512.0);
	//glVertex3dv(h1);
	glEnd();

}



void ff4() {
	double l[] = { 1,1,0 };
	double m[] = { 1,-2,0 };
	double n[] = { 9,1,0 };
	double o[] = { 9,-2,0 };
	double l1[] = { 1,1,2 };
	double m1[] = { 1,-2,2 };
	double n1[] = { 9,1,2 };
	double o1[] = { 9,-2,2 };
	double vn[] = { 0,0,0 };//вектор нормали

	double w[] = { 5,-5,0 };//центр
	double w1[] = { 5,-5,2 };
	float r = 5;
	double pi = 3.1415926535897932384;
	int numpoly = 180;
	double y = -5;
	glEnd();

	glBegin(GL_QUAD_STRIP);

	normal(l, n, m, vn);
	glNormal3dv(vn);

	for (int i = 0; i < numpoly + 1; i++)
	{
		if (r*cos(i*pi / numpoly) + 5 >= 1 && r*cos(i*pi / numpoly) + 5 <= 9) {

			glColor3f(0.5f, 0.5f, 0.5f);
			glTexCoord2d((380.0 + 50 * cos(i*3.14 / 180.0)) / 512.0, (512.0 - 90.0 + 50 * sin(i*3.14 / 180.0)) / 512.0);
			glVertex3f(r*cos(i*pi / numpoly) + 5, r*sin(i*pi / numpoly) - 5, 2);

			glColor3f(0.5f, 0.5f, 0.5f);
			glVertex3d(10 - 9 * i / 180, 1, 2);
		}
	}
	glEnd();
	glBegin(GL_TRIANGLES);

	glVertex3d(1, 1, 2);
	glVertex3d(1, -2, 2);
	glVertex3d(3, 1, 2);
	glEnd();
	
	/*glBegin(GL_QUAD_STRIP);

	double o22[] = { 5, -5,0 }; //центр окружности
	double o21[] = { 5, -5,0 }; //центр окружности
float r = 5; //радиус окружности
	double point[] = { 0,0,0 }; //точка дуги, x и y выбираем в цикле

	double point1[] = { 0,0,2 }; //точка дуги, x и y выбираем в цикле
	int alpha = -4 + asin(4 / 6.02) * 180 / 3.14;
	int betta = 184 - asin(4 / 6.02) * 180 / 3.14;
	int alala = 0;

	double pred[] = { -2, 4, 5 };//сначала тут d1
	double pred1[] = { -2, 4, -5 };//сначала тут d

	for (int i = alpha; i < betta; i++) //вогнутость
	{
		alala++;
		point[0] = o21[0] + r*cos(i*3.14 / 180);
		point[1] = o21[1] + r*sin(i*3.14 / 180);
		point1[0] = o22[0] + r*cos(i*3.14 / 180);
		point1[1] = o22[1] + r*sin(i*3.14 / 180);

		normal(point, pred, pred1, vn);
		glNormal3dv(vn);

		glColor3f(1.0f, 1.0f, 0.0f); //желтый
	//	glTexCoord2d((395.0 - (102 / 97)*alala) / 512.0, (512.0 - 300.0) / 512.0);
		glTexCoord2d(111.0 / 512.0, 1 - 40.0 / 512.0);
		glVertex3dv(point1);
		glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
		glTexCoord2d(236.0 / 512.0, 1 - 60.0 / 512.0);
		//glTexCoord2d((395.0 - (102 / 97)*alala) / 512.0, (512.0 - 200.0) / 512.0);
		glVertex3dv(point);

		pred[0] = point[0];
		pred[1] = point[1];
		pred1[0] = point1[0];
		pred1[1] = point1[1];
	}
	glEnd();

*/glBegin(GL_QUAD_STRIP);

	normal(o, n, m, vn);
	glNormal3dv(vn);

	for (int i = 0; i < numpoly + 1; i++)
	{
		if (r*cos(i*pi / numpoly) + 5 >= 1 && r*cos(i*pi / numpoly) + 5 <= 9) {

			glColor3f(0.5f, 0.5f, 0.5f);
		//	glTexCoord2d((286.5 + 37.0 * cos(i*3.14 / 180.0)) / 512.0, (512.0 - 78.0 + 37.0*sin(i*3.14 / 180)) / 512.0);
			glTexCoord2d((380.0 + 50*cos(i*3.14 / 180.0)) / 512.0, (512.0 - 90.0 + 50*sin(i*3.14 / 180.0)) / 512.0);
			glVertex3f(r*cos(i*pi / numpoly) + 5, r*sin(i*pi / numpoly) - 5, 0);
		//	glTexCoord2d(236.0 / 512.0, 1 - 60.0 / 512.0);
		//glTexCoord2d((350.0 + (60.0 / 97.0)*i) / 512.0, 1.0 - 65.0 / 512.0);
			glColor3f(0.5f, 0.5f, 0.5f);
			glVertex3d(10 - 9 * i / 180, 1, 0);
		}
	}
	glEnd();
	glBegin(GL_TRIANGLES);

	glVertex3d(1, 1, 0);
	glVertex3d(1, -2, 0);
	glVertex3d(3, 1, 0);
	glEnd();
	// боковой изгиб

	glBegin(GL_QUAD_STRIP);

	glColor3f(1.0f, 0.5f, 0.3f);
	//glVertex3d(9, 1, 0);

	glColor3f(1.0f, 0.5f, 0.3f);
	//glVertex3d(9, 1, 2);


	normal(m, n1, n, vn);
	glNormal3dv(vn);
	for (int i = 0; i < numpoly + 1; i++)
	{
		if (r*cos(i*pi / numpoly) + 5 >= 1 && r*cos(i*pi / numpoly) + 5 <= 9) {

			glColor3f(1.0f, 0.5f, 0.3f);
			glTexCoord2d(111.0 / 512.0, 1 - 40.0 / 512.0);
			glVertex3f(r*cos(i*pi / numpoly) + 5, r*sin(i*pi / numpoly) - 5, 0);
			glTexCoord2d(236.0 / 512.0, 1 - 60.0 / 512.0);
			glColor3f(1.0f, 0.5f, 0.3f);
			glVertex3f(r*cos(i*pi / numpoly) + 5, r*sin(i*pi / numpoly) - 5, 2);

		}
	}
	glEnd();

	glBegin(GL_QUADS);

	normal(n, n1, l, vn);
	glNormal3dv(vn);

	glTexCoord2d(0, 1 - 60.0 / 512.0);
	glVertex3dv(n);
	glTexCoord2d(0, 1 - 40.0 / 512.0);
	glVertex3dv(n1);
	glTexCoord2d(80.0 / 512.0, 1 - 40.0 / 512.0);
	glVertex3dv(l1);
	glTexCoord2d(80.0 / 512.0, 1 - 60.0 / 512.0);
	glVertex3dv(l);



	normal(m, m1, l, vn);
	glNormal3dv(vn);

	glTexCoord2d(80.0 / 512.0, 1 - 60.0 / 512.0);
	glVertex3dv(o);
	glTexCoord2d(80.0 / 512.0, 1 - 40.0 / 512.0);
	glVertex3dv(o1);
	glTexCoord2d(110.0 / 512.0, 1 - 40.0 / 512.0);
	glVertex3dv(n1);
	glTexCoord2d(110.0 / 512.0, 1 - 60.0 / 512.0);
	glVertex3dv(n);

	glEnd();
	

}
void ff1()
{
	double a[] = { 1,4,0 };
	double b[] = { 1,-7,0 };

	double e[] = { 1,-2,0 };
	double f[] = { -2,-2,0 };

	double g[] = { 1,1,0 };
	double h[] = { -2,1,0 };

	double c[] = { -2,4,0 };
	double d[] = { -2,-7,0 };

	double a1[] = { 1,4,2 };
	double b1[] = { 1,-7,2 };

	double e1[] = { 1,-2,2 };
	double f1[] = { -2,-2,2 };
	double g1[] = { 1,1,2 };
	double h1[] = { -2,1,2 };

	double c1[] = { -2,4,2 };
	double d1[] = { -2,-7,2 };

	double vn[] = { 0,0,0 };//вектор нормали

	glBegin(GL_TRIANGLES);


	normal(d, b, f, vn);
	glNormal3dv(vn);

	glVertex3dv(b);
	glVertex3dv(f);
	glVertex3dv(d);

	glVertex3dv(e);
	glVertex3dv(b);
	glVertex3dv(f);


	glVertex3dv(g);
	glVertex3dv(e);
	glVertex3dv(f);

	glVertex3dv(g);
	glVertex3dv(f);
	glVertex3dv(h);


	glVertex3dv(g);
	glVertex3dv(h);
	glVertex3dv(c);

	glVertex3dv(g);
	glVertex3dv(c);
	glVertex3dv(a);


	normal(f1, b1, d1, vn);
	glNormal3dv(vn);

	glVertex3dv(b1);
	glVertex3dv(f1);
	glVertex3dv(d1);

	glVertex3dv(e1);
	glVertex3dv(b1);
	glVertex3dv(f1);



	glVertex3dv(g1);
	glVertex3dv(e1);
	glVertex3dv(f1);

	glVertex3dv(g1);
	glVertex3dv(f1);
	glVertex3dv(h1);

	glVertex3dv(g1);
	glVertex3dv(h1);
	glVertex3dv(c1);

	glVertex3dv(g1);
	glVertex3dv(c1);
	glVertex3dv(a1);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.3f);

	normal(d1, b1, d, vn);
	glNormal3dv(vn);

	glVertex3dv(b);
	glVertex3dv(b1);
	glVertex3dv(d1);
	glVertex3dv(d);

	normal(h1, f1, h, vn);
	glNormal3dv(vn);


	glVertex3dv(h);
	glVertex3dv(h1);
	glVertex3dv(f1);
	glVertex3dv(f);

	normal(g1, a1, g, vn);
	glNormal3dv(vn);


	glVertex3dv(g);
	glVertex3dv(g1);
	glVertex3dv(a1);
	glVertex3dv(a);


	glEnd();
}
void ff2() {
	double a[] = { -2,-7,0 };
	double e[] = { -2,-2,0 };
	double j[] = { -8,-7,0 };
	double a1[] = { -2,-7,2 };
	double e1[] = { -2,-2,2 };
	double j1[] = { -8,-7,2 };
	double vn[] = { 0,0,0 };//вектор нормали

	glBegin(GL_TRIANGLES);


	glColor3f(0.5f, 0.5f, 0.5f);

	normal(j, a, e, vn);
	glNormal3dv(vn);

	glVertex3dv(a);
	glVertex3dv(e);
	glVertex3dv(j);

	normal(e1, a1, j1, vn);
	glNormal3dv(vn);

	glVertex3dv(a1);
	glVertex3dv(e1);
	glVertex3dv(j1);
	glEnd();

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.3f);

	normal(j1, a1, j, vn);
	glNormal3dv(vn);

	glVertex3dv(a);
	glVertex3dv(a1);
	glVertex3dv(j1);
	glVertex3dv(j);


	normal(e, e1, j, vn);
	glNormal3dv(vn);

	glVertex3dv(j);
	glVertex3dv(j1);
	glVertex3dv(e1);
	glVertex3dv(e);
	glEnd();

}
void ff3() {
	double e[] = { 1,-2,0 };
	double k[] = { 3,-7,0 };
	double b[] = { 1,-7,0 };
	double e1[] = { 1,-2,2 };
	double k1[] = { 3,-7,2 };
	double b1[] = { 1,-7,2 };
	double vn[] = { 0,0,0 };//вектор нормали
	glBegin(GL_TRIANGLES);
	glColor3f(0.5f, 0.5f, 0.5f);

	normal(k, e, b, vn);
	glNormal3dv(vn);

	glVertex3dv(e);
	glVertex3dv(k);
	glVertex3dv(b);

	normal(b1, e1, k1, vn);
	glNormal3dv(vn);

	glVertex3dv(e1);
	glVertex3dv(k1);
	glVertex3dv(b1);
	glEnd();

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.3f);

	normal(b1, e1, b, vn);
	glNormal3dv(vn);

	glVertex3dv(e);
	glVertex3dv(e1);
	glVertex3dv(k1);
	glVertex3dv(k);

	normal(b, k1, k, vn);
	glNormal3dv(vn);

	glVertex3dv(b);
	glVertex3dv(b1);
	glVertex3dv(k1);
	glVertex3dv(k);
	glEnd();

}
void ff5() {

	double h[] = { -2,1,0 };
	double m[] = { -7,4,0 };
	double c[] = { -2,4,0 };
	double h1[] = { -2,1,2 };
	double m1[] = { -7,4,2 };
	double c1[] = { -2,4,2 };
	double vn[] = { 0,0,0 };//вектор нормали

	glBegin(GL_TRIANGLES);

	glColor3f(0.5f, 0.5f, 0.5f);


	normal(m, h, c, vn);
	glNormal3dv(vn);

	glVertex3dv(h);
	glVertex3dv(m);
	glVertex3dv(c);

	normal(c1, h1, m1, vn);
	glNormal3dv(vn);

	glVertex3dv(h1);
	glVertex3dv(m1);
	glVertex3dv(c1);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.3f);


	normal(m1, h1, m, vn);
	glNormal3dv(vn);

	glVertex3dv(h);
	glVertex3dv(h1);
	glVertex3dv(m1);
	glVertex3dv(m);
	glEnd();
}

void fun_low() //нижнее основание
{
	double a[] = { 1,4,0 };
	double b[] = { 1,-7,0 };
	double e[] = { 1,-2,0 };
	double f[] = { -2,-2,0 };
	double g[] = { 1,1,0 };
	double h[] = { -2,1,0 };
	double c[] = { -2,4,0 };
	double d[] = { -2,-7,0 };
	double j[] = { -8,-7,0 };
	double k[] = { 3,-7,0 };
	double m[] = { -7,4,0 };
	double vn[] = { 0,0,0 };//вектор нормали

	glBegin(GL_TRIANGLES);
	glColor3f(0.5f, 0.5f, 0.5f);
	normal(f, d, g, vn);
	glNormal3dv(vn);
	//ff1
	glTexCoord2d(90.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(b);
	glTexCoord2d(140.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(f);
	glTexCoord2d(90.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(d);

	glTexCoord2d(140.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(e);
	glTexCoord2d(90.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(b);
	glTexCoord2d(140.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(f);

	glTexCoord2d(170.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(g);
	glTexCoord2d(140.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(e);
	glTexCoord2d(140.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(f);

	glTexCoord2d(170.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(g);
	glTexCoord2d(140.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(f);
	glTexCoord2d(170.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(h);

	glTexCoord2d(170.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(g);
	glTexCoord2d(170.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(h);
	glTexCoord2d(200.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(c);

	glTexCoord2d(170.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(g);
	glTexCoord2d(200.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(c);
	glTexCoord2d(200.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(a);
	//ff2
	glTexCoord2d(0, 1 - 140.0 / 512.0);
	glVertex3dv(d);
	glTexCoord2d(50.0 / 512.0, 1 - 140.0 / 512.0);
	glVertex3dv(f);
	glTexCoord2d(0, 1 - 80.0 / 512.0);
	glVertex3dv(j);
	//ff3
	glTexCoord2d(30.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(e);
	glTexCoord2d(80.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(k);
	glTexCoord2d(80.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(b);
	//ff5
	glTexCoord2d(30.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(h);
	glTexCoord2d(80.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(m);
	glTexCoord2d(30.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(c);
	glEnd();








	//для вогнутости
	//double n[] = { 2,2,-5 };
	//double o[] = { 2.5, 8,-5 }; //центр окружности
	//	float r = 6.02; //радиус окружности
	//double point[] = { 0,0,-5 }; //точка дуги, x и y выбираем в цикле
	//int alpha = 180 + asin(4 / 6.02) * 180 / 3.14;
	//int betta = 360 - asin(4 / 6.02) * 180 / 3.14;

	//glColor3f(1.0f, 0.5f, 0.0f); //оранжевый

	//double vn[] = { 0,0,0 };//вектор нормали
	//normal(g, b, a, vn);
	//glNormal3dv(vn);


	/*	glBegin(GL_QUADS);
	glTexCoord2d(420.0 / 512.0, (512.0 - 160.0) / 512.0);
	glVertex3dv(a);
	glTexCoord2d(420.0 / 512.0, (512.0 - 130.0) / 512.0);
	glVertex3dv(b);
	glTexCoord2d(300.0 / 512.0, (512.0 - 130.0) / 512.0);
	glVertex3dv(g);
	glTexCoord2d(300.0 / 512.0, (512.0 - 160.0) / 512.0);
	glVertex3dv(h);
	glEnd();
	*/
	/*glBegin(GL_QUAD_STRIP);
	glTexCoord2d(390.0 / 512.0, 1.0 - 100.0 / 512.0);
	glVertex3dv(d);
	glTexCoord2d(380.0 / 512.0, 1.0 - 130.0 / 512.0);
	glVertex3dv(c);
	double osn[] = { -1,1,-5 };
	int alala = 0;
	for (int i = alpha; i < betta; i++)
	{
	alala++;
	point[0] = o[0] + r*cos(i*3.14 / 180);
	point[1] = o[1] + r*sin(i*3.14 / 180);
	glTexCoord2d((345.0 - 60.2*cos(i*3.14 / 180.0)) / 512.0, (512.0 - 60.0 + 60.2*sin(i*3.14 / 180.0)) / 512.0);
	glVertex3dv(point);
	double osn[] = { -1 + 0.06186*alala,1,-5 };
	glTexCoord2d((380.0 - (60.0 / 97.0)*alala) / 512.0, 1.0 - 130.0 / 512.0); //исправь
	glVertex3dv(osn);
	}
	glTexCoord2d(300.0 / 512.0, 1.0 - 100.0 / 512.0);
	glVertex3dv(e);
	glTexCoord2d(320.0 / 512.0, 1.0 - 130.0 / 512.0);
	glVertex3dv(f);
	glEnd();*/

	fun_low_vip(); //выпуклость на нижнем основании
}

void fun_up() //верхнее основание
{
	double a1[] = { 1,4,2 };
	double b1[] = { 1,-7,2 };
	double e1[] = { 1,-2,2 };
	double f1[] = { -2,-2,2 };
	double g1[] = { 1,1,2 };
	double h1[] = { -2,1,2 };
	double c1[] = { -2,4,2 };
	double d1[] = { -2,-7,2 };
	double j1[] = { -8,-7,2 };
	double k1[] = { 3,-7,2 };
	double m1[] = { -7,4,2 };
	double vn[] = { 0,0,0 };//вектор нормали

	normal(g1, d1, f1, vn);
	glNormal3dv(vn);

	glBegin(GL_TRIANGLES);
	//ff1
	glTexCoord2d(90.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(b1);
	glTexCoord2d(140.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(f1);
	glTexCoord2d(90.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(d1);

	glTexCoord2d(140.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(e1);
	glTexCoord2d(90.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(b1);
	glTexCoord2d(140.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(f1);

	glTexCoord2d(170.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(g1);
	glTexCoord2d(140.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(e1);
	glTexCoord2d(140.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(f1);

	glTexCoord2d(170.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(g1);
	glTexCoord2d(140.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(f1);
	glTexCoord2d(170.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(h1);

	glTexCoord2d(170.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(g1);
	glTexCoord2d(170.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(h1);
	glTexCoord2d(200.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(c1);

	glTexCoord2d(170.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(g1);
	glTexCoord2d(200.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(c1);
	glTexCoord2d(200.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(a1);
	//ff2
	glTexCoord2d(0, 1 - 140.0 / 512.0);
	glVertex3dv(d1);
	glTexCoord2d(50.0 / 512.0, 1 - 140.0 / 512.0);
	glVertex3dv(f1);
	glTexCoord2d(0, 1 - 80.0 / 512.0);
	glVertex3dv(j1);
	//ff3
	glTexCoord2d(30.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(e1);
	glTexCoord2d(80.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(k1);
	glTexCoord2d(80.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(b1);
	//ff5
	glTexCoord2d(30.0 / 512.0, 1 - 80.0 / 512.0);
	glVertex3dv(h1);
	glTexCoord2d(80.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(m1);
	glTexCoord2d(30.0 / 512.0, 1 - 110.0 / 512.0);
	glVertex3dv(c1);
	glEnd();

	fun_up_vip(); //выпуклость на верхнем основании
}


void fun_side() //боковая сторона
{

	//точки нижнего основания

	double a[] = { 1,4,0 };
	double b[] = { 1,-7,0 };
	double e[] = { 1,-2,0 };
	double f[] = { -2,-2,0 };
	double g[] = { 1,1,0 };
	double h[] = { -2,1,0 };
	double c[] = { -2,4,0 };
	double d[] = { -2,-7,0 };
	double j[] = { -8,-7,0 };
	double k[] = { 3,-7,0 };
	double m[] = { -7,4,0 };

	//точки верхнего основания

	double a1[] = { 1,4,2 };
	double b1[] = { 1,-7,2 };
	double e1[] = { 1,-2,2 };
	double f1[] = { -2,-2,2 };
	double g1[] = { 1,1,2 };
	double h1[] = { -2,1,2 };
	double c1[] = { -2,4,2 };
	double d1[] = { -2,-7,2 };
	double j1[] = { -8,-7,2 };
	double k1[] = { 3,-7,2 };
	double m1[] = { -7,4,2 };

	double vn[] = { 0,0,0 };//вектор нормал
	//точки для дуги выпуклости

	double o11[] = { -3, 3.99,0 };
	double o12[] = { -3, 3.99,2 };

	int r1 = 4;
	//	int r1 = 10; //радиус окружности
	double point11[] = { 0,0,0 }; //точка дуги, x и y выбираем в цикле
	double point12[] = { 0,0,2 }; //точка дуги, x и y выбираем в цикле

	int alpha1 = 0;
	int betta1 = 180;
	//соединяем точки верхнего и нижнего основания

	glBegin(GL_QUAD_STRIP);

	int alala = 0;

	double pred[] = { 0, 0, 2 };//сначала тут d1
	double pred1[] = { 0, 0, 0 };//сначала тут d


	for (int i = betta1; i >= alpha1; i--) //выпуклость
	{
		alala++;
		point11[0] = o11[0] + r1 * cos(i*3.14 / 180);
		point11[1] = o11[1] + r1 * sin(i*3.14 / 180);
		point12[0] = o12[0] + r1 * cos(i*3.14 / 180);
		point12[1] = o12[1] + r1 * sin(i*3.14 / 180);

		normal(point12, pred, pred1, vn);
		glNormal3dv(vn);

		//glTexCoord2d((238.0 - (127 / 73)*alala) / 512.0, (512.0 - 512.0) / 512.0);
	    glTexCoord2d(111.0 / 512.0, 1 - 40.0 / 512.0);
		
		glVertex3dv(point11);
		glColor3f(1.0f, 0.5f, 0.3f);
		//glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	  // glTexCoord2d((238.0 - (127 / 73)*alala) / 512.0, (512.0 - 492.0) / 512.0);
		glTexCoord2d(236.0 / 512.0, 1 - 60.0 / 512.0);
		glVertex3dv(point12);
		glColor3f(1.0f, 0.5f, 0.3f);
		//glColor3f(1.0f, 1.0f, 0.0f); //желтый

		pred[0] = point12[0];
		pred[1] = point12[1];
		pred1[0] = point11[0];
		pred1[1] = point11[1];
	}
	glEnd();



	glColor3f(1.0f, 0.5f, 0.3f);

	normal(d1, b1, d, vn);
	glNormal3dv(vn);

	glVertex3dv(b);
	glVertex3dv(b1);
	glVertex3dv(d1);
	glVertex3dv(d);


	glBegin(GL_QUADS);

	//glColor3f(1.0f, 0.5f, 0.3f);

	normal(d1, b1, d, vn);
	glNormal3dv(vn);

	glTexCoord2d(60.0 / 512.0, 1 - 20.0 / 512.0);
	glVertex3dv(b);
	glTexCoord2d(60.0 / 512.0, 1);
	glVertex3dv(b1);
	glTexCoord2d(90.0 / 512.0, 1);
	glVertex3dv(d1);
	glTexCoord2d(90.0 / 512.0, 1 - 20.0 / 512.0);
	glVertex3dv(d);

	normal(h1, f1, h, vn);
	glNormal3dv(vn);

	glTexCoord2d(208.0 / 512.0, 1 - 20.0 / 512.0);
	glVertex3dv(h);
	glTexCoord2d(208.0 / 512.0, 1);
	glVertex3dv(h1);
	glTexCoord2d(238.0 / 512.0, 1);
	glVertex3dv(f1);
	glTexCoord2d(238.0 / 512.0, 1 - 20.0 / 512.0);
	glVertex3dv(f);

	normal(g1, a1, g, vn);
	glNormal3dv(vn);

	
	glTexCoord2d(178.0 / 512.0, 1 - 20.0 / 512.0);
	glVertex3dv(g);
	glTexCoord2d(178.0 / 512.0, 1);
	glVertex3dv(g1);
	glTexCoord2d(208.0 / 512.0, 1);
	glVertex3dv(a1);
	glTexCoord2d(208.0 / 512.0, 1 - 20.0 / 512.0);
	glVertex3dv(a);




	glColor3f(1.0f, 0.5f, 0.3f);

	normal(j1, d1, j, vn);
	glNormal3dv(vn);
	glTexCoord2d(0, 1-20.0/512.);
	glVertex3dv(d);
	glTexCoord2d(0.0, 1);
	glVertex3dv(d1);
	glTexCoord2d(60.0 / 512.0, 1);
	glVertex3dv(j1);
	glTexCoord2d(60.0 / 512.0, 1 - 20.0 / 512.0);
	glVertex3dv(j);


	normal(f, f1, j, vn);
	glNormal3dv(vn);
	glTexCoord2d(420.0 / 512.0, 1 - 20.0 / 512.0);
	glVertex3dv(j); //угол верхнего треугольника
	glTexCoord2d(420.0 / 512.0, 1);
	glVertex3dv(j1);
	glTexCoord2d(498.0 / 512.0,1);
	glVertex3dv(f1);
	glTexCoord2d(498.0 / 512.0, 1 - 20.0 / 512.0);
	glVertex3dv(f);


	glColor3f(1.0f, 0.5f, 0.3f);

	normal(b1, e1, b, vn);
	glNormal3dv(vn);
	glTexCoord2d(120.0 / 512.0, 1 - 20.0 / 512.0);
	glVertex3dv(e);// нижний треугольник 
	glTexCoord2d(120.0 / 512.0, 1);
	glVertex3dv(e1);
	glTexCoord2d(178.0 / 512.0, 1);
	glVertex3dv(k1);
	glTexCoord2d(178.0 / 512.0, 1 - 20.0 / 512.0);
	glVertex3dv(k);

	normal(b, k1, k, vn);
	glNormal3dv(vn);


	glTexCoord2d(90.0 /512.0, 1 - 20.0 / 512.0);
	glVertex3dv(b);
	glTexCoord2d(90.0 / 512.0, 1 );
	glVertex3dv(b1);
	glTexCoord2d(120.0 / 512.0, 1);
	glVertex3dv(k1);
	glTexCoord2d(120.0 / 512.0, 1 - 20.0 / 512.0);
	glVertex3dv(k);


	glColor3f(1.0f, 0.5f, 0.3f);


	normal(m1, h1, m, vn);
	glNormal3dv(vn);
	glTexCoord2d(364.0 / 512.0, 1 - 20.0 / 512.0);
	glVertex3dv(h); // верхний треугольник у полукруга
	glTexCoord2d(364.0 / 512.0, 1);
	glVertex3dv(h1); 
	glTexCoord2d(420.0 / 512.0, 1);
	glVertex3dv(m1);
	glTexCoord2d(420.0 / 512.0, 1 - 20.0 / 512.0);
	glVertex3dv(m);

	glEnd();

}

void fun_circle(void) //круг внутри призмы
{
	double center[] = { -2,4,1 };
	GLfloat theta;
	GLfloat pi = acos(-1.0); //пи
	GLfloat radius = 1.0f; // радиус

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.8f, 0.3f, 0.0f); //красный
	glNormal3f(0, 0, 1);

	glTexCoord2d(0.0625, 0.9375);
	glVertex3f(center[0], center[1], 1.0f);

	for (GLfloat a = 0.0f; a <= 360.0f; a++) {
		theta = pi * a / 180.0f;

		GLdouble x = 0.0625 + 0.0625 * cos(theta);
		GLdouble y = 0.9375 + 0.0625 * sin(theta);

		glTexCoord2d(x, y);
		glVertex3f(radius * cos(theta) + center[0], radius * sin(theta) + center[1], 1.0f);
	}
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


	//альфаналожение
	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); 
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  


	//Начало рисования квадратика станкина
	/*double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };



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

	glEnd();*/


	//glRotatef(ugol++, 0, 0, 1);
	//glTranslatef(-2.0f, 0.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, texId[0]);
	fun_circle();
	glBindTexture(GL_TEXTURE_2D, texId[1]);

	//fun_low_vip();
	fun_side();
	fun_low_vip();
	//build_circle();
	fun_low();
	ff4();
	//fun_up();
	glColor4f(0.5f, 0.5f, 0.5f, 0.0);
    fun_up_vip();
	fun_up();
	glColor4f(0.5f, 0.5f, 0.5f, 1.0);


	



	//fun_low(); //нижнее основание
	//fun_side(); //боковая сторона

	//fun_circle(); //круг внутри призмы

	//glColor4f(1.0f, 1.0f, 0.0f, 0.5f);
	//fun_up(); //верхнее основание
	//glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	

				//конец рисования квадратика станкина


				//текст сообщения вверху слева, если надоест - закоментировать, или заменить =)
	char c[350];  //максимальная длина сообщения
	sprintf_s(c, "(T)Текстуры - %d\n(L)Свет - %d\n\nУправление светом:\n"
		"G - перемещение в горизонтальной плоскости,\nG+ЛКМ+перемещение по вертикальной линии\n"
		"R - установить камеру и свет в начальное положение\n"
		"F - переместить свет в точку камеры\n"
		"A - Включить/выключить альфа наложение", textureMode, lightMode);
	ogl->message = std::string(c);




}   //конец тела функции

