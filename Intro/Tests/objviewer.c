#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

/* Variables for rotation */
float rotX = 0.0f, rotY = 0.0f;

/* Variables for camera movement */
float camX = 0.0f, camY = 0.0f, camZ = 5.0f;

/* OBJ file data structure */
typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    int v1, v2, v3;
} Face;

Vertex *vertices = NULL;
Face *faces = NULL;
int vertexCount = 0, faceCount = 0;

/* Load OBJ file */
void loadOBJ(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Unable to open the file %s\n", filename);
        exit(1);
    }

    char line[128];
    while (fgets(line, 128, file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            vertexCount++;
            vertices = realloc(vertices, vertexCount * sizeof(Vertex));
            sscanf(line, "v %f %f %f", &vertices[vertexCount - 1].x, &vertices[vertexCount - 1].y, &vertices[vertexCount - 1].z);
        } else if (line[0] == 'f') {
            faceCount++;
            faces = realloc(faces, faceCount * sizeof(Face));
            sscanf(line, "f %d %d %d", &faces[faceCount - 1].v1, &faces[faceCount - 1].v2, &faces[faceCount - 1].v3);
        }
    }

    fclose(file);
}

/* Render the OBJ model */
void renderOBJ()
{
    glColor3f(1.0f, 1.0f, 1.0f); // Set object color to white

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < faceCount; i++) {
        glVertex3f(vertices[faces[i].v1 - 1].x, vertices[faces[i].v1 - 1].y, vertices[faces[i].v1 - 1].z);
        glVertex3f(vertices[faces[i].v2 - 1].x, vertices[faces[i].v2 - 1].y, vertices[faces[i].v2 - 1].z);
        glVertex3f(vertices[faces[i].v3 - 1].x, vertices[faces[i].v3 - 1].y, vertices[faces[i].v3 - 1].z);
    }
    glEnd();
}

/* Display function */
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(camX, camY, camZ,  // Camera position
              camX, camY, 0.0f,  // Look at point (straight ahead)
              0.0f, 1.0f, 0.0f); // Up direction

    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    renderOBJ();

    glutSwapBuffers();
}

/* Keyboard controls for rotation and movement */
void keyboard(unsigned char key, int x, int y)
{
    float speed = 0.1f;

    switch (key) {
    case 'w': // Move forward
        camZ -= speed;
        break;
    case 's': // Move backward
        camZ += speed;
        break;
    case 'a': // Move left
        camX -= speed;
        break;
    case 'd': // Move right
        camX += speed;
        break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_RIGHT:
        rotY += 5.0f;
        break;
    case GLUT_KEY_LEFT:
        rotY -= 5.0f;
        break;
    case GLUT_KEY_UP:
        rotX += 5.0f;
        break;
    case GLUT_KEY_DOWN:
        rotX -= 5.0f;
        break;
    }
    glutPostRedisplay();
}

/* Main function */
int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <filename.obj>\n", argv[0]);
        return 1;
    }

    loadOBJ(argv[1]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OBJ Viewer with WASD Controls");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    glutMainLoop();

    free(vertices);
    free(faces);

    return 0;
}
