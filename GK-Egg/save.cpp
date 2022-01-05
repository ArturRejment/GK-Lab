#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include<windows.h>
#include<cmath>
#include<cstdlib>
#include<time.h>
#include<iostream>
using namespace std;

#define M_PI 3.14

typedef float point3[3];
point3** eggColors; // Kolory dla jajka

static GLfloat viewer[] = { 0.0, 0.0, 30.0 };

static GLfloat lightPosition[] = { 10.0, 10.0, 10.0 };
static GLfloat lightPosition2[] = { 10.0, 10.0, 10.0 };

static GLfloat theta = 0.0;   // kąt pierwszego źródła światła
static GLfloat phi = 0.0;
static GLfloat theta2 = 0.0;   // kąt drugiego źródła światła
static GLfloat phi2 = 0.0;

static GLfloat pix2angle;     // przelicznik pikseli na stopnie

// mysz
static GLint status = 0;       // stan klawiszy myszy
							   // 0 - nie naciśnięto żadnego klawisza
							   // 1 - naciśnięty zostać lewy klawisz
							   // 2 - naciśnięty został prawy klawisz

// x, y
static int x_pos_old = 0;
static int y_pos_old = 0;

static int delta_x = 0;
static int delta_y = 0;

// zoom
static GLfloat zoom = 0;
static GLfloat R = 10;

// object type
static int objType = 1; // 1 - czajnik, jajko

int model = 1;  // 1 - punkty, 2 - siatka, 3 - wypełnione trójkąty


// ściany piramidy
bool wall1 = true;
bool wall2 = true;
bool wall3 = true;
bool wall4 = true;
bool wall5 = true;
bool base = true;

GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat)
{
	// Struktura dla nag��wka pliku  TGA
#pragma pack(1)
	typedef struct
	{
		GLbyte    idlength;
		GLbyte    colormaptype;
		GLbyte    datatypecode;
		unsigned short    colormapstart;
		unsigned short    colormaplength;
		unsigned char     colormapdepth;
		unsigned short    x_orgin;
		unsigned short    y_orgin;
		unsigned short    width;
		unsigned short    height;
		GLbyte    bitsperpixel;
		GLbyte    descriptor;
	}TGAHEADER;
#pragma pack(8)

	FILE* pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte* pbitsperpixel = NULL;

	// Warto�ci domy�lne zwracane w przypadku b��du
	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;

	fopen_s(&pFile, FileName, "rb");
	if (pFile == NULL)
		return NULL;

	// Przeczytanie nag��wka pliku
	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

	// Odczytanie szeroko�ci, wysoko�ci i g��bi obrazu
	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;

	// Sprawdzenie, czy g��bia spe�nia za�o�one warunki (8, 24, lub 32 bity)
	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;

	// Obliczenie rozmiaru bufora w pami�ci
	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

	// Alokacja pami�ci dla danych obrazu
	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));

	if (pbitsperpixel == NULL)
		return NULL;

	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
	{
		free(pbitsperpixel);
		return NULL;
	}

	// Ustawienie formatu OpenGL
	switch (sDepth)
	{
	case 3:
		*ImFormat = GL_BGR_EXT;
		*ImComponents = GL_RGB8;
		break;
	case 4:
		*ImFormat = GL_BGRA_EXT;
		*ImComponents = GL_RGBA8;
		break;
	case 1:
		*ImFormat = GL_LUMINANCE;
		*ImComponents = GL_LUMINANCE8;
		break;
	};
	fclose(pFile);

	return pbitsperpixel;
}

// ---------------EGG----------------------
void drawEggWithPoints(point3** arr, int n)
{
	glBegin(GL_POINTS);
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			glVertex3fv(arr[i][j]);
		}
	}
	glEnd();
}

void drawEggWithLines(point3** arr, int n)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (i + 1 < n) {
				glBegin(GL_LINES);
				glVertex3fv(arr[i][j]);
				glVertex3fv(arr[i + 1][j]);
				glEnd();
			}

			if (j + 1 < n) {
				glBegin(GL_LINES);
				glVertex3fv(arr[i][j]);
				glVertex3fv(arr[i][j + 1]);
				glEnd();
			}
		}
	}
}

void drawEggWithTriangles(point3** array, point3** vect, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if ((j + 1) != n) {
				glBegin(GL_TRIANGLES);

				glNormal3fv(vect[i][j]);
				glTexCoord2f((float)i / n, (float)j / n);
				glVertex3fv(array[i][j]);

				glNormal3fv(vect[(i+1)%n][j]);
				glTexCoord2f((float)(i+1) / n, (float)j / n);
				glVertex3fv(array[(i + 1) % n][j]);

				glNormal3fv(vect[i][j+1]);
				glTexCoord2f((float)i / n, (float)(j+1) / n);
				glVertex3fv(array[i][j + 1]);
				glEnd();

				glBegin(GL_TRIANGLES);

				glNormal3fv(vect[(i + 1) % n][j]);
				glTexCoord2f((float)(i+1) / n, (float)j / n);
				glVertex3fv(array[(i + 1) % n][j]);

				glNormal3fv(vect[(i + 1) % n][j + 1]);
				glTexCoord2f((float)(i+1) / n, (float)(j+1) / n);
				glVertex3fv(array[(i + 1) % n][j + 1]);

				glNormal3fv(vect[i][j + 1]);
				glTexCoord2f((float)i / n, (float)(j+1) / n);
				glVertex3fv(array[i][j + 1]);
				glEnd();
			}
			else {
				if (i > 0) {
					glBegin(GL_TRIANGLES);

					glNormal3fv(vect[i][j]);
					glTexCoord2f((float)i / n, (float)j / n);
					glVertex3fv(array[i][j]);

					glNormal3fv(vect[(i + 1) % n][j]);
					glTexCoord2f((float)(i+1) / n, (float)j / n);
					glVertex3fv(array[(i + 1) % n][j]);

					glNormal3fv(vect[n - i][0]);
					glTexCoord2f((float)(n-1)/n, 0.0f);
					glVertex3fv(array[n - i][0]);
					glEnd();
				}
				glBegin(GL_TRIANGLES);

				glNormal3fv(vect[(i + 1) % n][j]);
				glTexCoord2f((float)(i+1) / n, (float)j / n);
				glVertex3fv(array[(i + 1) % n][j]);

				glNormal3fv(vect[(n - i) % n][0]);
				glTexCoord2f((float)(n-1) / n, 0.0f);
				glVertex3fv(array[(n - i) % n][0]);

				glNormal3fv(vect[n - i - 1][0]);
				glTexCoord2f((float)(n-i-1) / n, 0.0f);
				glVertex3fv(array[n - i - 1][0]);
				glEnd();
			}
		}
	}
	glEnd();
}

// ---------------- Piramida ----------------------
void Pyramid()
{
	if (wall1) {
		glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.0f, 4.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-4.0f, -4.0f, 4.0f);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(4.0f, -4.0f, 4.0f);
		glEnd();
	}

	if (wall2) {
		glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.0f, 4.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(4.0f, -4.0f, 4.0f);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(4.0f, -4.0f, -4.0f);
		glEnd();
	}

	if (wall3) {
		glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.0f, 4.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(4.0f, -4.0f, -4.0f);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(-4.0f, -4.0f, -4.0f);
		glEnd();
	}

	if (wall4) {
		glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.0f, 4.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-4.0f, -4.0f, -4.0f);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(-4.0f, -4.0f, 4.0f);
		glEnd();
	}

	if (base) {
		glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-4.0f, -4.0f, 4.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-4.0f, -4.0f, -4.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(4.0f, -4.0f, -4.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(4.0f, -4.0f, 4.0f);
		glEnd();
	}
}

// ------------ Policz vector --------------
float getXu(float u, float v)
{
	return (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(M_PI * v);
}

float getXv(float u, float v)
{
	return M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * sin(M_PI * v);
}

float getYu(float u, float v)
{
	return 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u;
}

float getYv(float u, float v)
{
	return 0;
}

float getZu(float u, float v)
{
	return (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * sin(M_PI * v);
}

float getZv(float u, float v)
{
	return (-M_PI) * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * cos(M_PI * v);
}

void Egg()
{
	int n = 50;
	point3** arr = new point3 * [n];
	point3** vect = new point3 * [n];
	for (int i = 0; i < n; i++)
	{
		arr[i] = new point3[n];
		vect[i] = new point3[n];
	}

	float u, v, vectX, vectY, vectZ, vectorLength;
	GLfloat xu, xv, yu, yv, zu, zv;
	for (int i = 0; i < n; i++)
	{
		u = float(i) / (n - 1);
		for (int j = 0; j < n; j++)
		{
			v = float(j) / (n - 1);
			arr[i][j][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45.0 * u) * cos(M_PI * v);
			arr[i][j][1] = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2) - 5.0;
			arr[i][j][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45.0 * u) * sin(M_PI * v);

			vectX = getYu(u, v) * getZv(u, v) - getZu(u, v) * getYv(u, v);
			vectY = getZu(u, v) * getXv(u, v) - getXu(u, v) * getZv(u, v);
			vectZ = getXu(u, v) * getYv(u, v) - getYu(u, v) * getXv(u, v);

			vectorLength = sqrt(vectX * vectX + vectY * vectY + vectZ * vectZ);
			if (vectorLength == 0)
				vectorLength = 1;

			vect[i][j][0] = vectX / vectorLength;
			vect[i][j][1] = vectY / vectorLength;
			vect[i][j][2] = vectZ / vectorLength;

			if (i >= (n / 2))
			{
				vect[i][j][0] *= -1;
				vect[i][j][1] *= -1;
				vect[i][j][2] *= -1;
			}

			if (i == 0 || i == n - 1)
			{
				vect[i][j][0] = 0;
				vect[i][j][1] = -1;
				vect[i][j][2] = 0;
			}
		}
	}

	if (model == 1)
		drawEggWithPoints(arr, n);
	else if (model == 2)
		drawEggWithLines(arr, n);
	else if (model == 3)
		drawEggWithTriangles(arr, vect, n);
}


// -----------------F ZWROTNE -------------------------
void Mouse(int btn, int state, int x, int y)
{


	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;
		y_pos_old = y;
		status = 1;
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;
		y_pos_old = y;
		status = 2;
	}
	else
		status = 0;
}


void Motion(GLsizei x, GLsizei y)
{

	delta_x = x - x_pos_old;
	delta_y = y - y_pos_old;

	x_pos_old = x;
	y_pos_old = y;

	glutPostRedisplay();
}


void Axes(void)
{

	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);

	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);

	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();

}

void generateEggColors(int n)
{
	srand(time(NULL));
	eggColors = new point3 * [n];
	int i, j;
	for (i = 0; i < n; i++)
	{
		eggColors[i] = new point3[n];
	}

	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			eggColors[i][j][0] = (rand() % 1001) / 1000.0;
			eggColors[i][j][1] = (rand() % 1001) / 1000.0;
			eggColors[i][j][2] = (rand() % 1001) / 1000.0;
		}
	}
}

void MyInit(void)
{
	GLbyte* pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;

	glEnable(GL_CULL_FACE);

	pBytes = LoadTGAImage("../tekstury/M3_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);

	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);

	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLfloat mat_ambient[] = { 1.0, 0.0, 1.0, 1.0 };
	GLfloat mat_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };

	GLfloat mat_shininess = { 20.0 };
	GLfloat light_position[] = { 10.0, 10.0, 10.0, 1.0 };

	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat light_ambient2[] = { 0.1, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 0.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

	GLfloat att_constant = { 1.0 };
	GLfloat att_linear = { 0.05 };
	GLfloat att_quadratic = { 0.001 };

	// Light 1
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);


	glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
	glEnable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny
	glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
	glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora

}


void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();


	if (status == 1)                     // jeśli lewy klawisz myszy wciśnięty
	{
		if (objType == 1)
		{
			theta += delta_x * pix2angle;
			phi += delta_y * pix2angle;
		}
		else if (objType == 2)
		{
			theta += (delta_x * pix2angle) * 0.01;
			phi += (delta_y * pix2angle) * 0.01;
		}

	}
	else if (status == 2)			// jeśli prawy klawisz myszy wciśnięty
	{
		zoom += delta_y / 20;
		if (zoom > 10.0) zoom = 10.0;
		if (zoom < -10.0) zoom = -10.0;

		R += delta_y / 10;
		if (R > 15.0) R = 15.0;
		if (R < 3.0) R = 3.0;
	}

	if (objType == 1)
	{
		viewer[0] = 0.0;
		viewer[1] = 0.0;
		viewer[2] = 10.0;
		R = 10.0;
	}
	else if (objType == 2)
	{
		zoom = 0;
		if (theta > 2.0 * M_PI) theta -= 2.0 * M_PI;
		if (theta < 0.0) theta += 2.0 * M_PI;
		if (phi > 2.0 * M_PI) phi -= 2.0 * M_PI;
		if (phi < 0.0) phi += 2.0 * M_PI;

		viewer[0] = R * cos(theta) * cos(phi);
		viewer[1] = R * sin(phi);
		viewer[2] = R * sin(theta) * cos(phi);
	}

	gluLookAt(viewer[0], viewer[1], viewer[2] + zoom, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


	if (objType == 1) {
		glRotatef(theta, 0.0, 1.0, 0.0);  //obrót obiektu o nowy kąt
		glRotatef(phi, 1.0, 0.0, 0.0);
	}


	gluLookAt(viewer[0], viewer[1], viewer[2] + zoom, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


	glColor3f(1.0f, 1.0f, 1.0f);
	if (objType == 1)
		Pyramid();
	else
		Egg();


	glutSwapBuffers();

}

void keys(unsigned char key, int x, int y)
{
	if (key == 'p') model = 1;
	if (key == 'w') model = 2;
	if (key == 's') model = 3;
	if (key == 'c')
	{
		objType = 1;
		theta = 0.0;
		phi = 0.0;
		zoom = 0.0;
		R = 10.0;
	}
	if (key == 'j')
	{
		objType = 2;
		theta = 0.0;
		phi = 0.0;
		zoom = 0.0;
		R = 10.0;
	}
	if (key == '1')
		wall1 = !wall1;
	if (key == '2')
		wall2 = !wall2;
	if (key == '3')
		wall3 = !wall3;
	if (key == '4')
		wall4 = !wall4;
	if (key == '5')
		wall5 = !wall5;

	RenderScene();
}


void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(80, 1.0, 1.0, 130.0);


	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);


	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

}


int main(void)
{
	generateEggColors(50);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(900, 900);

	glutCreateWindow("Oświetlenie scen 3D");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);

	glutMouseFunc(Mouse);
	// Ustala funkcję zwrotną odpowiedzialną za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcję zwrotną odpowiedzialną za badanie ruchu myszy
	glutKeyboardFunc(keys);

	cout << "Press 'c' for pyramid" << endl;
	cout << "Press 'j' for egg" << endl;
	cout << "Press keys from 1 to 5 to enable/disable walls" << endl;

	MyInit();

	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
	return 0;

}