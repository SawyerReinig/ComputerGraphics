
#include <GL/glut.h>

void display(){

static GLfloat coolColor[4] = {1, 1, 0.1, 0.1};

 glClear(GL_COLOR_BUFFER_BIT);
 glBegin(GL_POLYGON);
 glVertex2f(0.0,0.5);
 glVertex2f(0.5,-0.5);
 glVertex2f(-0.5,-0.5);

 glEnd();
 glFlush();
}

int main(int argc,char* argv[ ])
{
 glutInit(&argc,argv);
 glutInitDisplayMode()
 glutCreateWindow("Hello World");
 glutDisplayFunc(display);
 glutMainLoop();
}

