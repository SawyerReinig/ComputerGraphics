/*
 *  3D Objects
 *
 *  Demonstrates how to draw objects in 3D.
 *
 *  Key bindings:
 *  m/M        Cycle through different sets of objects
 *  a          Toggle axes
 *  arrows     Change view angle
 *  0          Reset view angle
 *  ESC        Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
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

const double worldSize = 1.8; 
const int waveResolution = 100; // Number of segments in the wave
float wavePositionZ = worldSize; // Initial z position of the wave
double t = 0; 

#define PI 3.14159265358979323846

int th=130;          //  Azimuth of view angle
int ph=20;          //  Elevation of view angle
double zh=0;       //  Rotation of teapot
int axes=0;        //  Display axes
int mode=0;        //  What to display
const char* text[] = {"Cuboids","Spheres","FlatPlane Outline","FlatPlane Fill","SolidPlane","Icosahedron DrawElements","Icosahedron DrawArrays","Icosahedron VBO","Scene"};

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.14159265/180))
#define Sin(x) (sin((x)*3.14159265/180))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
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
 *  Check for OpenGL errors
 */
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
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
void DrawCurlingWave(double time, double startpos) {
   double adjustedTime = time + startpos;

   glPushMatrix();
   double wavePosition = fmod(adjustedTime,worldSize+1); 
   glTranslated(0,0,wavePosition-worldSize+0.15);  
   glScalef(0.99, 0.15, -0.1);
   //  float radius = 0.5f; // Radius of the wave curl
    float curlHeight = 0.5f; // Height of the wave curl
    float waveLength = 2 * PI; // Length of one full wave cycle

    // Draw the wave on the positive side (+worldSize)
    glBegin(GL_QUAD_STRIP);
   glColor3ub(30, 150, 220); 

    float topPoints1[waveResolution + 1][3]; // Store the top points of the first wave
    float topPoints2[waveResolution + 1][3]; // Store the top points of the second wave

    for (int i = 0; i <= waveResolution; i++) {
        float t = (float)i / waveResolution;
        float z = -worldSize + 2.0f * worldSize * t; // Spread the wave along the z-axis
        float x = worldSize; // Position along x-axis

        // Main wave profile
        float yMain = sin(t * waveLength) * 0.2f; // Main wave oscillation

        // Curling part of the wave
        float curlFactor = sin(t * PI); // Create a curling factor that peaks at the center of the wave
        float curlY = yMain + curlFactor * curlHeight;

        // Bottom part of the wave (water surface)
        glVertex3f(x, 0.0f, z);
        // Top curling part of the wave
        glVertex3f(worldSize, curlY, z);

        // Store top points for later use in connecting waves
        topPoints1[i][0] = worldSize;
        topPoints1[i][1] = curlY;
        topPoints1[i][2] = z;
    }
    glEnd();

    // Draw the wave on the negative side (-worldSize)
    glBegin(GL_QUAD_STRIP);
   glColor3ub(30, 150, 220); 

    for (int i = 0; i <= waveResolution; i++) {
        float t = (float)i / waveResolution;
        float z = -worldSize + 2.0f * worldSize * t; // Spread the wave along the z-axis
        float x = -worldSize; // Position along negative x-axis

        // Main wave profile
        float yMain = sin(t * waveLength) * 0.2f; // Main wave oscillation

        // Curling part of the wave
        float curlFactor = sin(t * PI); // Create a curling factor that peaks at the center of the wave
        float curlY = yMain + curlFactor * curlHeight;

        // Bottom part of the wave (water surface)
        glVertex3f(x, 0.0f, z);
        // Top curling part of the wave
        glVertex3f(-worldSize, curlY, z);

        // Store top points for later use in connecting waves
        topPoints2[i][0] = -worldSize;
        topPoints2[i][1] = curlY;
        topPoints2[i][2] = z;
    }
    glEnd();

    // Connect the top points of the two waves with GL_QUADS
    glBegin(GL_QUADS);
   glColor3ub(30, 150, 220); 

    for (int i = 0; i < waveResolution; i++) {
        // Top points of the first wave
        glVertex3f(topPoints1[i][0], topPoints1[i][1], topPoints1[i][2]);
        glVertex3f(topPoints1[i + 1][0], topPoints1[i + 1][1], topPoints1[i + 1][2]);

        // Corresponding top points of the second wave
        glVertex3f(topPoints2[i + 1][0], topPoints2[i + 1][1], topPoints2[i + 1][2]);
        glVertex3f(topPoints2[i][0], topPoints2[i][1], topPoints2[i][2]);
    }
    glEnd();
    glPopMatrix(); 
}

void cubeHelper(){
   glBegin(GL_QUADS);
   //  Front
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
}


void Pole(float posx, float posy, float posz,float radius, double height, int slices) {
    float angleStep = (2.0f * PI) / slices;

    // Draw the cylinder sides
    glColor3ub(27, 70, 224); //color of the umbrella posts
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; i++) {
        float angle = i * angleStep;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        // Bottom vertex (at y = 0)
        glVertex3f(x+posx, posy, z+posz);

        // Top vertex (at y = height)
        glVertex3f(x+posx, height+posy, z+posz);
    }
    glEnd();

    // Draw the top circle
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(posx, height+posy, posz); // Center of the top circle
    for (int i = 0; i <= slices; i++) {
        float angle = i * angleStep;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        glVertex3f(x+posx, height+posy, z+posz);
    }
    glEnd();

    // Draw the bottom circle
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(posx, posy, posz); // Center of the bottom circle
    for (int i = 0; i <= slices; i++) {
        float angle = i * angleStep;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        glVertex3f(x+posx, posy, z+posz);
    }
    glEnd();
}

static void BeachMat(double x,double y,double z, double th, double rx, double ry, double rz)
{
   //  Save transformation
   glPushMatrix();
   glColor3f(x/2,y,z/2);
   glTranslated(x,y,z);
   glRotated(th,rx,ry,rz);
   glScaled(0.08, 0.03, 0.2); 
   //  Cube points
   cubeHelper();

   //pillow
   glPushMatrix();
   glColor3f(1,1,1);
   glTranslated(0,1,0.7);
   glRotated(th,rx,ry,rz);
   glScaled(0.8, 0.1, 0.2); 
   cubeHelper();
   glPopMatrix();
   glPopMatrix();

}

static void UmbrellaVertex(double th,double ph)
{
   // glColor3f(Cos(th)*Cos(th) , Sin(ph)*Sin(ph) , Sin(th)*Sin(th));
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph)-0.1 , Cos(th)*Cos(ph));
}

static void DrawWater(int type,double x,double y,double z)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glColor3ub(24, 142, 201); 
   //  3d water
   glPushMatrix(); 
   glTranslated(0,-worldSize/4,-worldSize/2);
   glScaled(worldSize,worldSize/4,worldSize/2); 
   glBegin(GL_QUADS);
   //  Front
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   glPopMatrix(); 
   //  Undo transformations
   glPopMatrix();
}

static void DrawBeach(int type,double x,double y,double z)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   //  The flat plane
   glColor3f(0.94f, 0.85f, 0.68f); //color for sand
   glPushMatrix(); 
   glTranslated(0,-worldSize/4+0.01,+worldSize/2);
   glScaled(worldSize,worldSize/3.9,worldSize/2); 
   glBegin(GL_QUADS);
   //  Front
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   glPopMatrix(); 
   //  Undo transformations
   glPopMatrix();
}

float CurveFunction(float z, float startZ, float endZ)
    {
        float t = (z - startZ) / (endZ - startZ);
        return -t * t * 0.7f + t;
}


void DrawSolidBeach()
{
    // Beach dimensions
    float startX = -worldSize;
    float endX =  worldSize;
    float startZ =  0.0f;
    float endZ =  worldSize;
    float yBase =  0.00f;
    int numSegments = 50;
    float LeftPoints[numSegments+1][3];
    float RightPoints[numSegments+1][3]; //could optimize this by just adding an offset to LeftPoints

   //Hoping to create an array of all beach points, then make a polygon that connects all beach points to the ground inorder to have water tight shape


    glBegin(GL_QUAD_STRIP);
    glColor3f(0.96f, 0.87f, 0.70f); //color for sand
    for (int i = 0; i <= numSegments; ++i)
    {
      float t = (float)i / numSegments;
      float z = startZ + t * (endZ - startZ);
      float yOffset = CurveFunction(z, startZ, endZ);

      // Left vertex
      glVertex3f(startX, yBase + yOffset, z);
      LeftPoints[i][0] = startX; 
      LeftPoints[i][1] = yBase + yOffset; 
      LeftPoints[i][2] = z; 

      // Right vertex
      glVertex3f(endX, yBase + yOffset, z);
      RightPoints[i][0] = endX; 
      RightPoints[i][1] = yBase + yOffset; 
      RightPoints[i][2] = z; 
    }
    glEnd();
   //create the polygons to connect up the beach

   //draw the right side of beach
   glBegin(GL_POLYGON);
   glVertex3f( worldSize, 0.02, 0);
   for(int i = 0; i <= numSegments; i++){
      glVertex3f( RightPoints[i][0], RightPoints[i][1], RightPoints[i][2]); 
   } 
   glVertex3f( worldSize, 0.02, worldSize); //front right corner of world
   glEnd();

   //draw the left side of beach
   glBegin(GL_POLYGON);
   glVertex3f( -worldSize, 0.02, 0);
   for(int i = 0; i <= numSegments; i++){
      glVertex3f( LeftPoints[i][0], LeftPoints[i][1], LeftPoints[i][2]); 
   } 
   glVertex3f( -worldSize, 0.02, worldSize); //front right corner of world
   glEnd();

   //draw the back of the beach
   glBegin(GL_POLYGON);
   glVertex3f( worldSize, 0.02, worldSize);
   glVertex3f( RightPoints[numSegments][0], RightPoints[numSegments][1], RightPoints[numSegments][2]); 
   glVertex3f( LeftPoints[numSegments][0], LeftPoints[numSegments][1], LeftPoints[numSegments][2]); 
   glVertex3f( -worldSize, 0.02, worldSize); //front right corner of world
   glEnd();

}


static void CreateUmbrella(double x,double y,double z, double height,double angle, float rx, float ry, float rz, float scale){
   const int Rad = 40;
   glPushMatrix();
   glTranslated(x,y,z); 
   glRotatef(angle,rx,ry,rz);
   glScaled(scale, scale, scale); 


   Pole(0,0,0,0.02, height, 50); 

   glPushMatrix(); 
   glTranslated(0,-height,0);
   glScaled(0.75, 1.3, 0.75); 

   glBegin(GL_TRIANGLE_FAN);
   glColor3ub(24, 142, 201); 
   UmbrellaVertex(0,90);
   for (int th=0;th<=360;th+=Rad)
   {
      glColor3f(255,255,255); 
      UmbrellaVertex(th,90-Rad);
   }
   glEnd();
   glPopMatrix();
   glPopMatrix();

}

void display()
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Set view angle
   glRotatef(ph,1,0,0);
   glRotatef(th,0,1,0);
   //  Decide what to draw
   
   // this will be the main beach scene. 
   // FlatPlane(GL_POLYGON , 0,0,0);
   // If we raise the beach up as a curved surface, then when the waves hit the beach they will appear to disapate in size. 
   DrawWater(GL_POLYGON, 0,0,0); 
   DrawBeach(GL_POLYGON, 0,0,0); 
   DrawSolidBeach(); 
   CreateUmbrella(-0.4,0.2,0.4,0.42, 30, 5, -15, 0, 0.9); 
   CreateUmbrella(0.7,0.2,0.3,0.42, 15, -1, -3, 5, 0.6);
   CreateUmbrella(1.2,0.3,1,0.42, 0, -1, -3, 5, 1); 
   BeachMat(-0.6,0.18,0.33,25,-20,0,0);
   BeachMat(0.5,0.23,0.33,25,-20,0,0); 
   BeachMat(1,0.35,1,10,-1,0,0); 
   for(int i = 0; i < 9; i++){
      DrawCurlingWave(t/12,0.3111 * i);
   }      
   
   //  White
   glColor3f(1,1,1);
   //  Draw axes
   if (axes)
   {
      const double len=1.5;  //  Length of axes
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Five pixels from the lower left corner of the window
   glWindowPos2i(5,5);
   //  Print the text string
   Print("Angle=%d,%d    %s",th,ph,text[mode]);
   //  Render the scene
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
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
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   //  Switch display mode
   else if (ch == 'm')
      mode = (mode+1)%9;
   else if (ch == 'M')
      mode = (mode+8)%9;
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
   //  Orthogonal projection
   // const double dim=2.5;
   const double dim=2.5;

   double asp = (height>0) ? (double)width/height : 1;
   glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  GLUT calls this routine when there is nothing else to do
 */
void idle()
{
   t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360);
   glutPostRedisplay();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Sawyer Reinig Peaceful Beach");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   //  Tell GLUT to call "idle" when there is nothing else to do
   glutIdleFunc(idle);
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
   return 0;
}
