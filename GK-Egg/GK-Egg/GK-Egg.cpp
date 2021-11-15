#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include<cmath>
#include<cstdlib>
#include<time.h>

#define M_PI 3.14

typedef float point3[3];


static GLfloat theta[] = { 0.0, 0.0, 0.0 };
int model = 1;  // 1- punkty, 2- siatka, 3 - wypełnione trójkąty

void spinEgg()
{

    theta[0] -= 0.5;
    if (theta[0] > 360.0) theta[0] -= 360.0;

    theta[1] -= 0.5;
    if (theta[1] > 360.0) theta[1] -= 360.0;

    theta[2] -= 0.5;
    if (theta[2] > 360.0) theta[2] -= 360.0;

    glutPostRedisplay(); //odświeżenie zawartości aktualnego okna
}


void Axes(void)
{

    point3  x_min = { -5.0, 0.0, 0.0 };
    point3  x_max = { 5.0, 0.0, 0.0 };
    // początek i koniec obrazu osi x

    point3  y_min = { 0.0, -5.0, 0.0 };
    point3  y_max = { 0.0,  5.0, 0.0 };
    // początek i koniec obrazu osi y

    point3  z_min = { 0.0, 0.0, -5.0 };
    point3  z_max = { 0.0, 0.0,  5.0 };
    //  początek i koniec obrazu osi y
    glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
    glBegin(GL_LINES); // rysowanie osi x
    glVertex3fv(x_min);
    glVertex3fv(x_max);
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
    glBegin(GL_LINES);  // rysowanie osi y

    glVertex3fv(y_min);
    glVertex3fv(y_max);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
    glBegin(GL_LINES); // rysowanie osi z

    glVertex3fv(z_min);
    glVertex3fv(z_max);
    glEnd();

}

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

/*
void drawEggWithTriangles(point3** arr, int n, point3 **colors)
{
    glBegin(GL_LINES);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if ((j + 1) != n) {
                glColor3fv(colors[i][j]);
                glVertex3fv(arr[i][j]);
                glColor3fv(colors[(i + 1) % n][j]);
                glVertex3fv(arr[(i + 1) % n][j]);
                glColor3fv(colors[i][j + 1]);
                glVertex3fv(arr[i][j + 1]);

                glColor3fv(colors[(i + 1) % n][j]);
                glVertex3fv(arr[(i + 1) % n][j]);
                glColor3fv(colors[(i + 1) % n][j + 1]);
                glVertex3fv(arr[(i + 1) % n][j + 1]);
                glColor3fv(colors[i][j + 1]);
                glVertex3fv(arr[i][j + 1]);
            }
            else {
                if (i > 0)
                {
                    glColor3fv(colors[i][j]);
                    glVertex3fv(arr[i][j]);
                    glColor3fv(colors[(i + 1) % n][j + 1]);
                    glVertex3fv(arr[(i + 1) % n][j + 1]);
                    glColor3fv(colors[n - i][0]);
                    glVertex3fv(arr[n - i][0]);
                }
                glColor3fv(colors[(i + 1) % n][j]);
                glVertex3fv(arr[(i + 1) % n][j]);
                glColor3fv(colors[(n - i) % n][0]);
                glVertex3fv(arr[(n - i) % n][0]);
                glColor3fv(colors[n - i - 1][0]);
                glVertex3fv(arr[n - i - 1][0]);
            }
            glEnd();
        }
    }
}
*/


void Egg()
{
    int n = 50;
    point3 **arr = new point3*[n];
    for (int i = 0; i < n; i++)
        arr[i] = new point3[n];

    point3** colors = new point3 * [n];
    for (int i = 0; i < n; i++)
        colors[i] = new point3[n];

    srand(time(NULL));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            colors[i][j][0] = (rand() % 1000) / 1000.0;
            colors[i][j][1] = (rand() % 1000) / 1000.0;
            colors[i][j][2] = (rand() % 1000) / 1000.0;
        }
    }

    float u, v;
    for (int i = 0; i < n; i++)
    {
        u = float(i) / (n - 1);
        for (int j = 0; j < n; j++)
        {
            v = float(j) / (n - 1);
            arr[i][j][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45.0 * u) * cos(M_PI * v);
            arr[i][j][1] =  160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2) - 5.0;
            arr[i][j][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45.0 * u) * sin(M_PI * v);
        }
    }

    if (model == 1)
        drawEggWithPoints(arr, n);
    else if (model == 2)
        drawEggWithLines(arr, n);
   // else if (model == 3)
        //drawEggWithTriangles(arr, n, colors);
}


void RenderScene(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
   
    Axes();

    glRotatef(theta[0], 1.0, 0.0, 0.0);

    glRotatef(theta[1], 0.0, 1.0, 0.0);

    glRotatef(theta[2], 0.0, 0.0, 1.0);

    Egg();

    glFlush();

    glutSwapBuffers();

}

void keys(unsigned char key, int x, int y)
{
    if (key == 'p') model = 1;
    if (key == 'w') model = 2;
    if (key == 's') model = 3;

    RenderScene();
}


void MyInit(void)
{

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{

    GLfloat AspectRatio;
    if (vertical == 0) 
        vertical = 1;
    glViewport(0, 0, horizontal, vertical); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
  
    if (horizontal <= vertical)
        glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);
    else
        glOrtho(-7.5 * AspectRatio, 7.5 * AspectRatio, -7.5, 7.5, 10.0, -10.0);

    glMatrixMode(GL_MODELVIEW);
                          

    glLoadIdentity();

}


int main(void)
{

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 900);
    glutCreateWindow("Układ współrzędnych 3-D");
    glutKeyboardFunc(keys);
    glutIdleFunc(spinEgg);
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);     
    MyInit();
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
}
