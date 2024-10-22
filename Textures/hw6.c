//Don't forget to use gorod light smoothing for curves
#include "CSCIx229.h"


const double worldSize = 1.8; 
const int waveResolution = 100; // Number of segments in the wave
float wavePositionZ = worldSize; // Initial z position of the wave
double t = 0; 

int move=0;       //  Move light
int th=70;          //  Azimuth of view angle
int ph=20;          //  Elevation of view angle
// double zh=0;       //  Rotation of teapot
int axes=0;        //  Display axes
int PerspectiveMode=2;        //  What to display
int fov=90;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world
double Ex; 
double Ey; 
double Ez; 

float posX = -1.7, posY = 0.5, posZ = 0.67;  // Camera position
float dirX = 0.9, dirY = -0.14, dirZ = -0.45;   // Direction vector the camera is looking at
float angle = 1.1;                     // Angle for rotating the camera horizontally
float pitch = -0.15;                     // Vertical angle (pitch)
float speed = 0.1;                     // Speed of movement

//Varables for lighting:
int light     =   1;  // Lighting
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   =  50;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        = -90;  // Light azimuth
float ylight  = 0.8;  // Elevation of light
unsigned int texture[7]; // Texture names
unsigned int SkyboxTextures[6]; // Texture names



const char* text[] = {"Ortho","Perspective","First Person"};

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

// Function to initialize SDL2 audio
int initAudio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Initialize SDL_mixer with a frequency of 44100 Hz, 16-bit audio, 2 channels, and 2048 byte chunks
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return -1;
    }

    return 0;
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

   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, texture[2]);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Draw the wave on the positive side (+worldSize)
   glBegin(GL_QUAD_STRIP);
   glNormal3f(1, 0, 0);

   glColor3ub(30, 150, 220); 

    float WavePointsRight[waveResolution + 1][3]; // Store the top points of the first wave
    float WavePointsLeft[waveResolution + 1][3]; // Store the top points of the second wave

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
        glTexCoord2f(t, 0.0f); glVertex3f(x, 0.0f, z);
        // Top curling part of the wave
        glTexCoord2f(t, 1.0f); glVertex3f(x, curlY, z);

        // Store top points for later use in connecting waves
        WavePointsRight[i][0] = x;
        WavePointsRight[i][1] = curlY;
        WavePointsRight[i][2] = z;
    }
    glEnd();

    // Draw the wave on the negative side (-worldSize)
   glBegin(GL_QUAD_STRIP);
   glNormal3f(-1, 0, 0);

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
        glTexCoord2f(t, 0.0f); glVertex3f(x, 0.0f, z);
        // Top curling part of the wave
        glTexCoord2f(t, 1.0f); glVertex3f(x, curlY, z);
        // Store top points for later use in connecting waves
        WavePointsLeft[i][0] = x;
        WavePointsLeft[i][1] = curlY;
        WavePointsLeft[i][2] = z;
    }
    glEnd();

    // Connect the top points of the two waves with GL_QUADS
   glBegin(GL_QUADS);
   glColor3ub(30, 150, 220); 
   float dz = waveLength/waveResolution; 
    for (int i = 0; i < waveResolution; i++) {

      //to calculate the normal for the actual wave part, we take the dz, which is just  wavelength/waveresolution
      //then we need to find the change in y, so we save the y from the last point and subtract it from the new y
      //then we do a cross product to get our final normal vector. 
      //excpet that we then have to normalize
      float dy = WavePointsRight[i + 1][1] - WavePointsRight[i][1];

      float normalX = 0.0f;  // Tangent in the z-y plane, no x-component
      float normalY = dz;   //flip the points and make y negative to solve for tangent vector
      float normalZ = -dy; 

      float length = sqrt(normalX*normalX + normalY*normalY + normalZ*normalZ);
      normalX /= length;
      normalY /= length;
      normalZ /= length;
      glNormal3f(normalX, normalY, normalZ);

      // Top points of the first wave
      // int randomInt = rand() %;
      // printf("Random integer: %d\n", randomInt);
      float textureCoord = i / (float)waveResolution * time ;  // Stretch texture smoothly across the entire wave
      float nextTextureCoord = (i + 1) / (float)waveResolution * time;


      glTexCoord2f(textureCoord, 0.0f);  // Bottom edge
      glVertex3f(WavePointsRight[i][0], WavePointsRight[i][1], WavePointsRight[i][2]);
      glTexCoord2f(nextTextureCoord, 0.0f);  // Bottom edge
      glVertex3f(WavePointsRight[i + 1][0], WavePointsRight[i + 1][1], WavePointsRight[i + 1][2]);

      // Corresponding top points of the second wave
      glTexCoord2f(nextTextureCoord, 1.0f);  // top edge
      glVertex3f(WavePointsLeft[i + 1][0], WavePointsLeft[i + 1][1], WavePointsLeft[i + 1][2]);
      glTexCoord2f(textureCoord, 1.0f);  // top edge
      glVertex3f(WavePointsLeft[i][0], WavePointsLeft[i][1], WavePointsLeft[i][2]);
    }

    glEnd();
   glDisable(GL_TEXTURE_2D);

    glPopMatrix(); 
}

void cubeHelper(int WhichTexture){

   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   if(WhichTexture != -1){
      glEnable(GL_TEXTURE_2D);
      // glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE); //old version that uses mode
      glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); //just trying out the two modes.
      glBindTexture(GL_TEXTURE_2D,texture[WhichTexture]); 
   }
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0.0, 0.0); glVertex3f(-1, -1,  1); // Bottom-left
   glTexCoord2f(1.0, 0.0); glVertex3f( 1, -1,  1); // Bottom-right
   glTexCoord2f(1.0, 1.0); glVertex3f( 1,  1,  1); // Top-right
   glTexCoord2f(0.0, 1.0); glVertex3f(-1,  1,  1); // Top-left
   // Back face
   glNormal3f(0, 0, -1);
   glTexCoord2f(0.0, 0.0); glVertex3f( 1, -1, -1); // Bottom-left
   glTexCoord2f(1.0, 0.0); glVertex3f(-1, -1, -1); // Bottom-right
   glTexCoord2f(1.0, 1.0); glVertex3f(-1,  1, -1); // Top-right
   glTexCoord2f(0.0, 1.0); glVertex3f( 1,  1, -1); // Top-left
   
   // Right face
   glNormal3f(1, 0, 0);
   glTexCoord2f(0.0, 0.0); glVertex3f( 1, -1,  1); // Bottom-left
   glTexCoord2f(1.0, 0.0); glVertex3f( 1, -1, -1); // Bottom-right
   glTexCoord2f(1.0, 1.0); glVertex3f( 1,  1, -1); // Top-right
   glTexCoord2f(0.0, 1.0); glVertex3f( 1,  1,  1); // Top-left
   
   // Left face
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0, 0.0); glVertex3f(-1, -1, -1); // Bottom-left
   glTexCoord2f(1.0, 0.0); glVertex3f(-1, -1,  1); // Bottom-right
   glTexCoord2f(1.0, 1.0); glVertex3f(-1,  1,  1); // Top-right
   glTexCoord2f(0.0, 1.0); glVertex3f(-1,  1, -1); // Top-left
   
   // Top face
   glNormal3f(0, 1, 0);
   glTexCoord2f(0.0, 0.0); glVertex3f(-1,  1,  1); // Bottom-left
   glTexCoord2f(1.0, 0.0); glVertex3f( 1,  1,  1); // Bottom-right
   glTexCoord2f(1.0, 1.0); glVertex3f( 1,  1, -1); // Top-right
   glTexCoord2f(0.0, 1.0); glVertex3f(-1,  1, -1); // Top-left
   
   // Bottom face
   glNormal3f(0, -1, 0);
   glTexCoord2f(0.0, 0.0); glVertex3f(-1, -1, -1); // Bottom-left
   glTexCoord2f(1.0, 0.0); glVertex3f( 1, -1, -1); // Bottom-right
   glTexCoord2f(1.0, 1.0); glVertex3f( 1, -1,  1); // Top-right
   glTexCoord2f(0.0, 1.0); glVertex3f(-1, -1,  1); // Top-left
   //End
   glEnd();
   glDisable(GL_TEXTURE_2D);

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
        glNormal3f(x,0,z);

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
   // glColor3f(x/2,y,z/2);
   glTranslated(x,y,z);
   glRotated(th,rx,ry,rz);
   glScaled(0.08, 0.03, 0.2); 
   //  Cube points
   cubeHelper(5);

   //pillow
   glPushMatrix();
   glColor3f(1,1,1);
   glTranslated(0,1,0.7);
   glRotated(th,rx,ry,rz);
   glScaled(0.8, 0.1, 0.2); 
   cubeHelper(-1);
   glPopMatrix();
   glPopMatrix();

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
   cubeHelper(2);
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
   cubeHelper(0);

   glPopMatrix(); 
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

   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, texture[0]);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glBegin(GL_QUAD_STRIP);
   // glColor3f(0.96f, 0.87f, 0.70f); //color for sand
    float prevz = startZ; 
    for (int i = 0; i <= numSegments; ++i)
    {
      float t = (float)i / numSegments;
      float z = startZ + t * (endZ - startZ);
      float yOffset = CurveFunction(z, startZ, endZ);


      // Calculate the tangent for the curve (approximate derivative of the curve)
      float dz = z - prevz;  // change in z direction
      float nextYOffset = CurveFunction(z + dz, startZ, endZ);  // Small step ahead for derivative
      float dy = nextYOffset - yOffset;  // Difference in y-values (slope)

      prevz = z; 

      // The normal is perpendicular to the surface tangent
      float normalX = 0.0f;  // Tangent in the z-y plane, no x-component
      float normalY = dz;   //flip the points and make y negative to solve for tangent vector
      float normalZ = -dy;  

      // Normalize the normal vector
      float length = sqrt(normalX*normalX + normalY*normalY + normalZ*normalZ);
      normalX /= length;
      normalY /= length;
      normalZ /= length;

      // Apply normals and draw vertices
      glNormal3f(normalX, normalY, normalZ);  // Set the normal


      // Left vertex
      glTexCoord2f(t, 0.0f);
      glVertex3f(startX, yBase + yOffset, z);
      LeftPoints[i][0] = startX; 
      LeftPoints[i][1] = yBase + yOffset; 
      LeftPoints[i][2] = z; 

      // Right vertex
      glTexCoord2f(t, 1.0f);
      glVertex3f(endX, yBase + yOffset, z);
      RightPoints[i][0] = endX; 
      RightPoints[i][1] = yBase + yOffset; 
      RightPoints[i][2] = z; 
    }
    glEnd();


   glBegin(GL_QUAD_STRIP);
   glNormal3f( 1, 0, 0);
   glTexCoord2f(0.0f, 0.0f);
   glVertex3f(worldSize, 0.02, 0);
   int xtiling = 3;
   for(int i = 0; i <= numSegments; i++){
      float t = (float)i / numSegments;

      glTexCoord2f(t*xtiling, 0.0f);
      glVertex3f( RightPoints[i][0], 0.02f, RightPoints[i][2]);
      glTexCoord2f(t*xtiling, 1.0f);
      glVertex3f( RightPoints[i][0], RightPoints[i][1], RightPoints[i][2]); 
 
   } 
      glTexCoord2f(1.0f, 0.0f);
      glVertex3f(worldSize, 0.02, worldSize);
   
   glEnd();

   //draw the left side of beach
   glBegin(GL_QUAD_STRIP);
   glNormal3f( -1, 0, 0);
   glTexCoord2f(0.0f, 0.0f);
   glVertex3f(-worldSize, 0.02, 0);
   for(int i = 0; i <= numSegments; i++){

      float t = (float)i / numSegments;
      glTexCoord2f(t*xtiling, 0.0f);
      glVertex3f( LeftPoints[i][0], 0.02f, LeftPoints[i][2]);
      glTexCoord2f(t*xtiling, 1.0f);
      glVertex3f( LeftPoints[i][0], LeftPoints[i][1], LeftPoints[i][2]);  

   } 
   glTexCoord2f(1.0f, 0.0f);
   glVertex3f( -worldSize, 0.02, worldSize); //front right corner of world
   glEnd();

   //draw the back of the beach
   glBegin(GL_POLYGON);
   glNormal3f( 0, 0, 1);
   float ztile = 10; 
   glTexCoord2f(0.0f, 0.0f);  // Bottom edge
   glVertex3f( worldSize, 0.02, worldSize);

   glTexCoord2f(0.0f, 1.0f);
   glVertex3f( RightPoints[numSegments][0], RightPoints[numSegments][1], RightPoints[numSegments][2]); 

   glTexCoord2f(ztile, 0.0f);
   glVertex3f( LeftPoints[numSegments][0], LeftPoints[numSegments][1], LeftPoints[numSegments][2]); 

   glTexCoord2f(ztile, 1.0f);
   glVertex3f( -worldSize, 0.02, worldSize); //front right corner of world
   glEnd();
   glDisable(GL_TEXTURE_2D);
}

static void UmbrellaVertex(double th,double ph)
{
   // glColor3f(Cos(th)*Cos(th) , Sin(ph)*Sin(ph) , Sin(th)*Sin(th));
   // glTexCoord2f(th, ph);
   double x = Sin(th) * Cos(ph);
   double y = Sin(ph) - 0.1;
   double z = Cos(th) * Cos(ph);

   glVertex3d(x, y, z);
}

static void CreateUmbrella(double x,double y,double z, double height,double angle, float rx, float ry, float rz, float scale){
   const int Rad = 40;
   glPushMatrix();
   glTranslated(x,y,z); 
   glRotatef(angle,rx,ry,rz);
   glScaled(scale, scale, scale); 


   Pole(0,0,0,0.02, height+0.3, 50); 

   glPushMatrix(); 
   glTranslated(0,-height,0);
   glScaled(0.75, 1.3, 0.75); 
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); //just trying out the two modes.
   glBindTexture(GL_TEXTURE_2D,texture[4]); 

   glBegin(GL_TRIANGLE_FAN);

   glColor3ub(242, 140, 44);

   glNormal3f(0, 1, 0);
   glTexCoord2f(0.5f, 0.5f);  // Center of texture at (0.5, 0.5)
   UmbrellaVertex(0,90);
   
   
   
   for (int th=0;th<=360;th+=Rad)
   {
      // glColor3f(148, 252, 3); 
      double nx = Sin(th) * Cos(90 - Rad);
      double nz = Cos(th) * Cos(90 - Rad);
      double ny = Sin(90 - Rad);  // Small tilt for the umbrella's rim

      // Normalize the normal vector
      double length = sqrt(nx * nx + ny * ny + nz * nz);
      nx /= length;
      ny /= length;
      nz /= length;
      glNormal3f(nx, ny, nz);

      double u = 0.5 + 0.5 * Sin(th);  // Texture u-coordinate
      double v = 0.5 + 0.5 * Cos(th);  // Texture v-coordinate
      glTexCoord2f(u, v);  // Set texture coordinates for this vertex
      UmbrellaVertex(th,90-Rad);
   }
   glEnd();
   glPopMatrix();
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);

}

void playSound(const char* filename) {
    Mix_Chunk* sound = Mix_LoadWAV(filename);
    if (!sound) {
        printf("Failed to load sound! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }

    // Play the sound (channel -1 means any available channel)
    Mix_PlayChannel(-1, sound, 0);
}
void DrawSkybox(float size) {
    // Disable depth writing to avoid skybox interfering with other objects
    glDepthMask(GL_FALSE);
    
    // Push the matrix to keep transformations isolated
    glPushMatrix();
    
    // Center the skybox on the camera position
    glTranslated(posX, posY, posZ);
    glEnable(GL_TEXTURE_2D);
    
    // Bind the skybox texture
    glBindTexture(GL_TEXTURE_2D, SkyboxTextures[2]);
    // Front face
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size, -size, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size, size, -size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, size, -size);
    glEnd();

    // Repeat for all sides (Back, Left, Right, Top, Bottom)
    // Back face
   glBindTexture(GL_TEXTURE_2D, SkyboxTextures[0]);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(size, -size, size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, -size, size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, size, size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(size, size, size);
    glEnd();

    // Left face
    glBindTexture(GL_TEXTURE_2D, SkyboxTextures[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, -size, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, size, -size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, size, size);
    glEnd();

    // Right face
    glBindTexture(GL_TEXTURE_2D, SkyboxTextures[5]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(size, -size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size, -size, size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size, size, size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(size, size, -size);
    glEnd();

    // Top face
    glBindTexture(GL_TEXTURE_2D, SkyboxTextures[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size, size, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size, size, size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, size, size);
    glEnd();

    // Bottom face
    glBindTexture(GL_TEXTURE_2D, SkyboxTextures[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size, -size, size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size, -size, -size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, -size, -size);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    // Restore depth mask
    glDepthMask(GL_TRUE);

    // Pop the matrix to restore transformations
    glPopMatrix();
}
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

static void ball(double x,double y,double z,double r)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball with yellow specular
   float yellow[]   = {1.0,1.0,0.0,1.0};
   float Emission[] = {0.0,0.0,0.01*emission,1.0};
   glColor3f(242, 140, 44);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);

   //  Bands of latitude
   for (int ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
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
   switch (PerspectiveMode){
      case 0:
         glRotatef(ph,1,0,0);
         glRotatef(th,0,1,0);
      break;
      case 1:
         Ex = -2*dim*Sin(th)*Cos(ph);
         Ey = +2*dim        *Sin(ph);
         Ez = +2*dim*Cos(th)*Cos(ph);
         gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
      break;
      case 2:
         gluLookAt(posX, posY, posZ,            // Camera position
              posX + dirX, posY + dirY, posZ + dirZ,  // Camera looking towards
              0.0, 1.0, 0.0); 
      break;
   }

   if (light){
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light position
      float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
      //  Draw light position as ball (still no lighting here)
      glColor3ub(242, 140, 44);
      ball(Position[0],Position[1],Position[2] , 0.1);
      //  OpenGL should normalize normal vectors
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  Location of viewer for specular calculations
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
      glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else{
      glDisable(GL_LIGHTING);
   }
   //SHAPES TO BE DRAWN SECTION
   DrawWater(GL_POLYGON, 0,0,0); //lighting solved
   DrawBeach(GL_POLYGON, 0,0,0); //lighting solved
   DrawSolidBeach(); //lighting solved
   CreateUmbrella(-0.4,0.2,0.4,0.42, 30, 5, -15, 0, 0.9); 
   CreateUmbrella(0.7,0.2,0.3,0.42, 15, -1, -3, 5, 0.6);
   CreateUmbrella(1.2,0.3,1,0.42, 0, -1, -3, 5, 1);
   BeachMat(-0.6,0.18,0.33,25,-20,0,0);//lighting solved
   BeachMat(0.5,0.16,0.33,25,-20,0,0); //lighting solved
   BeachMat(1,0.35,1,10,-1,0,0); //lighting solved
   DrawSkybox(10);
   for(int i = 0; i < 9; i++){
      DrawCurlingWave(t/12,0.3111 * i);
   }      
   //END OF SHAPES TO BE DRAWN
   //  White
   glDisable(GL_LIGHTING);
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
   // Print("Angle=%d,%d Dim=%.1f FOV=%d   %s",th,ph,dim,fov, text[PerspectiveMode]);
   // Print("posX%f, posY%f, posX%f, dirX%f, dirY%f, dirZ%f, Pitch%f, Angle%f",posX,posY,posZ,dirX, dirY, dirZ, pitch, angle);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s",
     th,ph,dim,fov,PerspectiveMode?"Perpective":"Orthogonal",light?"On":"Off");
   if (light)
   {
      glWindowPos2i(5,45);
      Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",local?"On":"Off",distance,ylight);
      glWindowPos2i(5,25);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
   }
   //  Render the scene
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   
   //  Orthogonal projection
   if (PerspectiveMode == 0)
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Perspective transformation
   else if (PerspectiveMode == 1)
      // gluPerspective(abs(fov),asp,dim/4,4*dim);
      gluPerspective(abs(fov),asp,dim/4,50); //increased to 50 for the skybox
   //First Person Perspective
   else if(PerspectiveMode == 2)
      // gluPerspective(abs(fov),asp,0.01,4*dim);
      gluPerspective(abs(fov),asp,0.01,50);

   

   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT){
      th += 5;
      angle += 0.05f;
      dirX = sin(angle);
      dirZ = -cos(angle);
   }
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT){
      th -= 5;
      angle -= 0.05f;
      dirX = sin(angle);
      dirZ = -cos(angle);
   }
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP){
      ph += 5;
      pitch += 0.05f;
      dirY = sin(pitch);
   }
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN){
      ph -= 5;
      pitch -= 0.05f;
      dirY = sin(pitch);
   }
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   Project();

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
   //WASD
   // Move forward (W)
   else if (ch == 'w' || ch == 'W') {
      posX += dirX * speed;
      posZ += dirZ * speed;
   }
   // Move backward (S)
   else if (ch == 's' || ch == 'S') {
      posX -= dirX * speed;
      posZ -= dirZ * speed;
   }
   // Move left (A)
   else if (ch == 'a' || ch == 'A') {
      posX += dirZ * speed;  // Move perpendicular to the direction
      posZ -= dirX * speed;
   }
   // Move right (D)
   else if (ch == 'd' || ch == 'D') {
      posX -= dirZ * speed;  // Move perpendicular to the direction
      posZ += dirX * speed;
   }

   //  Move light
   else if (ch == '<')
      zh += 3;
   else if (ch == '>')
      zh -= 3;


   //  Light elevation
   else if (ch=='[')
      ylight -= 0.1;
   else if (ch==']')
      ylight += 0.1;
   //  Ambient level
   else if (ch=='z' && ambient>0)
      ambient -= 5;
   else if (ch=='Z' && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (ch=='x' && diffuse>0)
      diffuse -= 5;
   else if (ch=='X' && diffuse<100)
      diffuse += 5;
   //  Specular level
   else if (ch=='c' && specular>0)
      specular -= 5;
   else if (ch=='C' && specular<100)
      specular += 5;
   //  Emission level
   else if (ch=='v' && emission>0)
      emission -= 5;
   else if (ch=='V' && emission<100)
      emission += 5;
   //  Shininess level
   else if (ch=='b' && shininess>-1)
      shininess -= 1;
   else if (ch=='B' && shininess<7)
      shininess += 1;
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   //toggle light movement
   else if (ch == 'm' || ch == 'M')
      move = 1-move;

   //move up and down
   else if (ch == 'e'|| ch == 'E') {
      posY += 0.1; 
   }
   else if (ch == 'q' || ch == 'Q') {
      posY -= 0.1; 
   }

   //  Toggle axes
   else if (ch == ' ')
      axes = 1-axes;
   //  Switch display mode
   else if (ch == 'p')
      PerspectiveMode = (PerspectiveMode+1)%3;
   else if (ch == 'P')
      PerspectiveMode = (PerspectiveMode+8)%3;

   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   //  Tell GLUT it is necessary to redisplay the scene

   shiny = shininess<0 ? 0 : pow(2.0,shininess);

   Project();
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
//  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   //  Set projection
   Project();

}

/*
 *  GLUT calls this routine when there is nothing else to do
 */
void idle()
{
   static int firstCall = 1;
    if (firstCall) {
        playSound("waves.mp3");
        firstCall = 0;
    }

   t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   if(move){
   zh = fmod(90*t,360);

   }
   glutPostRedisplay();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{

   if (initAudio() < 0) { //initialize SDL2 for audio
        return -1;
    }

   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Sawyer Reinig, Its getting hot in here");
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
   texture[0] = LoadTexBMP("BMPS/sand.bmp");
   texture[2] = LoadTexBMP("BMPS/water.bmp");
   texture[3] = LoadTexBMP("BMPS/vibrant.bmp");
   texture[4] = LoadTexBMP("BMPS/waves.bmp");
   texture[5] = LoadTexBMP("BMPS/sunset.bmp");
   SkyboxTextures[0] = LoadTexBMP("BMPS/skybox/backside.bmp");
   SkyboxTextures[1] = LoadTexBMP("BMPS/skybox/bottom.bmp");
   SkyboxTextures[2] = LoadTexBMP("BMPS/skybox/SemperFi.bmp");
   SkyboxTextures[3] = LoadTexBMP("BMPS/skybox/top.bmp");
   SkyboxTextures[4] = LoadTexBMP("BMPS/skybox/leftside.bmp");
   SkyboxTextures[5] = LoadTexBMP("BMPS/skybox/rightside.bmp");




   glutMainLoop();

   Mix_CloseAudio();
   SDL_Quit();
   return 0;
}
