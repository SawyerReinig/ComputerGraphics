/*
 *  Coordinates
 *
 *  Display 2, 3 and 4 dimensional coordinates in 3D.
 *
 *  Key bindings:
 *  1      2D coordinates
 *  2      2D coordinates with fixed Z value
 *  3      3D coordinates
 *  4      4D coordinates
 *  +/-    Increase/decrease z or w
 *  arrows Change view angle
 *  0      Reset view angle
 *  ESC    Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef USEGLEW
#include <GL/glew.h>
#endif
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
// Tell Xcode IDE to not gripe about OpenGL deprecation
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif
//  Default resolution
//  For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif

//  Globals
int LineLength = 40; 
double dim = 70;   // Dimension of orthogonal box
int th=0;       // Azimuth of view angle
int ph=0;       // Elevation of view angle
int mode=1;     // Dimension (1-4)
double Depth=0;     // Z variable
double w=1;     // W variable
const char* text[] = {"","2D","3D constant Z","3D","4D"};  // Dimension display text

/*  Lorenz Parameters  */
// double s  = 10;
// double b  = 2.6666;
// double r  = 28;
double b  = 2.6666;
double r  = 21;
double s  = 10;

double Prevx = 1, Prevy = 1, Prevz = 1;

double x = 4.346813, y = 2.06845, z = 20.624024;
double dt = 0.001;

int num_points = 50000;
float points[50000][3];

void generateLorenz()
{
   Prevx = x; 
   Prevy = y; 
   Prevz = z; 
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

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  // Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Print message to stderr and exit
 */
void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}

/*
 *  Check for OpenGL errors
 */
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

/*
 *  Display the scene
 */
void display()
{
   //  Clear the image
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   //  Reset previous transforms
   glLoadIdentity();
   //  Set view angle
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   //  Draw 2 pixel yellow points
   glColor3f(1,1,0);
   glPointSize(2);
   double t = glutGet(GLUT_ELAPSED_TIME)/10.0;

   switch (mode)
   {
   //  Two dimensions
   case 1:
      glBegin(GL_LINE_STRIP);
      for (int i = 0; i < num_points; i++)
      { 
         glColor3ub((i + (int)t) % 256, (i + (int)t) % 256, (i + (int)t) % 256);
         glVertex3f(points[i][0], points[i][1], points[i][2]);
      }
      glEnd();
      break;
   //  Three dimensions - constant Z
   case 2:
      glBegin(GL_LINE_STRIP);
      for (int i = 0; i < num_points; i++)
      { 
         glColor3ub((i + (int)t + 85) % 256, (i + (int)t+170) % 256, (i + (int)t) % 256);
         glVertex3f(points[i][0], points[i][1], points[i][2]);
      }
      glEnd();
      break;
   //  Three dimensions - variable Z
   case 3:
      glBegin(GL_LINE_STRIP);
      for (int i = 0; i < num_points; i++)
      { 
         glColor3ub((i + (int)t*2) % 256, (i + (int)t*3) % 256, (i + (int)t*4) % 256);
         glVertex3f(points[i][0], points[i][1], points[i][2]); 
      }
      glEnd();
      break;
   //  Four dimensions
   case 4:
      glBegin(GL_LINE_STRIP);
      for (int i = 0; i < num_points; i++)
      { 
         int Random = rand() % 1;
         int Random1 = rand()% 1;
         int Random2 = rand()% 1;

         glColor3ub((i + (int)t*2*Random) % 256, (i + (int)t*3*Random1) % 256, (i + (int)t*4*Random2) % 256);
         glVertex3f(points[i][0], points[i][1], points[i][2]);
      }
      glEnd();
      break;
   }
   //  Draw axes in white
   glColor3f(0,1,1);
   // glBegin(GL_LINES);
   glBegin(GL_LINES);

   glVertex3d(0,0,0);
   glVertex3d(LineLength,0,0);
   glVertex3d(0,0,0);
   glVertex3d(0,LineLength,0);
   glVertex3d(0,0,0);
   glVertex3d(0,0,LineLength);
   glEnd();
   //  Label axes
   glRasterPos3d(LineLength,0,0);
   Print("X");
   glRasterPos3d(0,LineLength,0);
   Print("Y");
   glRasterPos3d(0,0,LineLength);
   Print("Z");
   //  Display parameters
   glWindowPos2i(5,5);
   Print("View Angle=%d,%d, Radius=%f, X=%f, Y=%f, Z=%f",th,ph, r, Prevx, Prevy, Prevz);
   if (mode==2)
      Print("  z=%.1f",Depth);
   else if (mode==4)
      Print("  w=%.1f",w);
   //  Flush and swap
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}  

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Switch dimensions
   else if ('1'<=ch && ch<='4')
   {
      mode = ch-'0';
      if (mode==2) Depth = 0;
      if (mode==4) w = 1;
   }
   //increase the radius by 1 with +
   else if (ch == '+')
   {
      r += 1;
      // x = 1;
      // y = 1;
      // z = 1;
      generateLorenz();
   }
   //decrease the radius by 1 with -
   else if (ch == '-')
   {
      r -= 1;
      // x = 1;
      // y = 1;
      // z = 1;
      generateLorenz(); 
   }
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase azimuth by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease azimuth by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection box adjusted for the
   //  aspect ratio of the window
   double asp = (height>0) ? (double)width/height : 1;
   glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Start up GLUT and tell it what to do
 */

void idle()
{ 
   glutPostRedisplay();
}


int main(int argc,char* argv[])
{
   generateLorenz(); 
  //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window 
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   //  Request 500 x 500 pixel window
   glutInitWindowSize(500,500);
   //  Create the window
   glutCreateWindow("Sawyer Reinig Spinning Lorenz");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   glutIdleFunc(idle); // Register idle callback

   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   //  Return code
   return 0;
}
