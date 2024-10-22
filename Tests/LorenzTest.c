#include <GL/glut.h>
#include <stdio.h>

/* Lorenz Parameters */
double s = 10;
double b = 2.6666;
double r = 28;

/* Coordinates */
double x = 1, y = 1, z = 1;
double dt = 0.001;
int num_points = 50000;
float points[50000][3];

/* Initialize the Lorenz Attractor points */
void generateLorenz()
{
    for (int i = 0; i < num_points; i++)
    {
        double dx = s * (y - x);
        double dy = x * (r - z) - y;
        double dz = x * y - b * z;
        x += dt * dx;
        y += dt * dy;
        z += dt * dz;
        points[i][0] = x;
        points[i][1] = y;
        points[i][2] = z;
    }
}

/* OpenGL Display Function */
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < num_points; i++)
    {
        glVertex3f(points[i][0], points[i][1], points[i][2]);
    }
    glEnd();

    glutSwapBuffers();
}

/* OpenGL Reshape Function */
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

/* Keyboard interaction to adjust parameters */
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'r':
        r += 1;
        generateLorenz();
        glutPostRedisplay();
        break;
    case 'R':
        r -= 1;
        generateLorenz();
        glutPostRedisplay();
        break;
    case 's':
        s += 1;
        generateLorenz();
        glutPostRedisplay();
        break;
    case 'S':
        s -= 1;
        generateLorenz();
        glutPostRedisplay();
        break;
    case 'b':
        b += 0.1;
        generateLorenz();
        glutPostRedisplay();
        break;
    case 'B':
        b -= 0.1;
        generateLorenz();
        glutPostRedisplay();
        break;
    case 27: // Escape key
        exit(0);
        break;
    }
}

int main(int argc, char **argv)
{
    generateLorenz();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Lorenz Attractor");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
