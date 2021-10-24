#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <Windows.h>

void drawCarpet(int x, int y, int width, int iterations)
{
    if (iterations <= 0)
    {
        return;
    }
 
    width /= 3;

    glColor3f(255.0f, 255.0f, 255.0f);
    glRectf(x + width, y + width, x + width + width, y + width + width);
    glFlush();
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (i == 1 && j == 1)
            {
                continue;
            }
            drawCarpet(x + i * width, y + j * width, width, iterations-1);
        }
    }
}

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0f, 0.0f, 0.0f);
    glRectf(-121.0f, 121.0f, 121.0f, -121.0f);
    drawCarpet(-121, -121, 243, 4);
}


void MyInit(void)
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
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
        glOrtho(-200.0, 200.0, -200.0 / AspectRatio, 200.0 / AspectRatio, 1.0, -1.0);
    else
        glOrtho(-100.0 * AspectRatio, 100.0 * AspectRatio, -100.0, 100.0, 1.0, -1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}


int main(void)
{
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("Drugi program w OpenGL");
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    MyInit();
    glutMainLoop();
}
