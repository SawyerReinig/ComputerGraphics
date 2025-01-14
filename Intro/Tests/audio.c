#include <GL/glut.h>
#include <irrKlang.h>

// irrKlang namespace
using namespace irrklang;

// Define global variables for the audio engine and sound
ISoundEngine* soundEngine = NULL;
ISound* backgroundSound = NULL;

// Function to play sound with irrKlang
void playSound(const char* filename) {
    if (soundEngine) {
        backgroundSound = soundEngine->play2D(filename, true, false, true); // Looping enabled
    }
}

// OpenGL display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Draw a simple triangle
    glBegin(GL_TRIANGLES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(-1.0, -1.0, 0.0);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(1.0, -1.0, 0.0);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0.0, 1.0, 0.0);
    glEnd();

    glutSwapBuffers();
}

// OpenGL idle function to continuously play the sound
void idle() {
    if (backgroundSound && !backgroundSound->isFinished()) {
        // Sound is still playing
    } else if (backgroundSound) {
        // Sound has finished, restart it
        backgroundSound->setPlayPosition(0);
        backgroundSound->setIsPaused(false);
    }
    
    glutPostRedisplay();
}

// OpenGL initialization
void initOpenGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL with irrKlang Sound");

    // Initialize OpenGL
    initOpenGL();

    // Initialize irrKlang sound engine
    soundEngine = createIrrKlangDevice();

    if (!soundEngine) {
        printf("Could not initialize irrKlang sound engine\n");
        return 1; // Exit if the engine couldn't be created
    }

    // Play background music
    playSound("background_music.wav");

    // Set OpenGL callbacks
    glutDisplayFunc(display);
    glutIdleFunc(idle);

    // Enter the main loop
    glutMainLoop();

    // Cleanup irrKlang
    if (soundEngine) {
        soundEngine->drop(); // Release the engine
    }

    return 0;
}
