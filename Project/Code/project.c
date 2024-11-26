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
double spc=1;     //  Specular intensity
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        = -90;  // Light azimuth
float ylight  = 0.8;  // Elevation of light
unsigned int texture[7]; // Texture names
unsigned int SkyboxTextures[6]; // Texture names

int currentFrame = 0;
int frameRateDelay = 36;    //controlls playback speed for now
int AnimatePerson = 1; 
int frameCount = 0;
BodyFrame* motionData; 

// BodyLength Measurements: 
float upperArmLength = 2;
float forearmLength = 2;

int SuperHotModel;          //  Object display list
double ObjScale = 0.5; 

float RGBA[4] = {1,1,1,1};  //  Colors

typedef struct {
    float vertex[3];
    float normal;
} FaceDataIndex;

objVertex vertices[MAX_VERTICES];
float normals[MAX_NORMALS][3];
FaceDataIndex objDataIndices[MAX_NORMALS]; 

int vertex_count = 0;
int normal_count = 0;
int face_count = 0; 

// Function to parse the OBJ file
void loadOBJWithColor(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "v ", 2) == 0) { // Vertex line
            if (vertex_count >= MAX_VERTICES) {
                fprintf(stderr, "Error: Exceeded maximum number of vertices.\n");
                break;
            }

            objVertex vertex;
            sscanf(line, "v %f %f %f %f %f %f", 
                   &vertex.position[0], &vertex.position[1], &vertex.position[2],
                   &vertex.color[0], &vertex.color[1], &vertex.color[2]);
            vertices[vertex_count++] = vertex;
        } else if (strncmp(line, "vn ", 3) == 0) { // Normal line
            if (normal_count >= MAX_NORMALS) {
                fprintf(stderr, "Error: Exceeded maximum number of normals.\n");
                break;
            }

            sscanf(line, "vn %f %f %f",
                   &normals[normal_count][0], &normals[normal_count][1], &normals[normal_count][2]);
            normal_count++;
        } else if (strncmp(line, "f ", 2) == 0) { // Face line
    int vertex_indices[3];
    int normal_index;
    
    // Parse the face line to extract vertex and normal indices
    if (sscanf(line, "f %d//%d %d//%d %d//%d",
               &vertex_indices[0], &normal_index,
               &vertex_indices[1], &normal_index,
               &vertex_indices[2], &normal_index) != 6) {
        fprintf(stderr, "Error: Face line format not as expected.\n");
        continue;
    }
    objDataIndices[face_count].vertex[0] = vertex_indices[0];
    objDataIndices[face_count].vertex[1] = vertex_indices[1];
    objDataIndices[face_count].vertex[2] = vertex_indices[2];
    objDataIndices[face_count].normal = normal_index;
    face_count = face_count+1; 
printf("Face Count is: %d \n", face_count);
    
}
    }
     

    fclose(file);
}

// Function to print the parsed data
void printOBJData() {
    printf("Parsed Vertices:\n");
    for (int i = 0; i < vertex_count; i++) {
        printf("Vertex %d: Position(%f, %f, %f) Color(%f, %f, %f)\n", 
               i + 1,
               vertices[i].position[0], vertices[i].position[1], vertices[i].position[2],
               vertices[i].color[0], vertices[i].color[1], vertices[i].color[2]);
    }

    printf("\nParsed Normals:\n");
    for (int i = 0; i < normal_count; i++) {
        printf("Normal %d: (%f, %f, %f)\n",
               i + 1,
               normals[i][0], normals[i][1], normals[i][2]);
    }
}

void DrawObj(){
   for(int face = 0; face < face_count; face++){
      glPushMatrix();
      // glTranslated(face/300,face/300,face/300); 

      glBegin(GL_TRIANGLES);
      // For each vertex in the triangle
      for (int i = 0; i < 3; i++) {
         // Adjust for 1-based indexing in the OBJ file
         int v_idx = objDataIndices[face].vertex[i] - 1; 
         int n_idx = objDataIndices[face].normal - 1;

         if (v_idx >= vertex_count || n_idx >= normal_count || v_idx < 0 || n_idx < 0) {
            fprintf(stderr, "Error: Index out of bounds in face line.\n");
            continue;
         }
        
          
         
         // Set the normal for this vertex
         glNormal3f(normals[n_idx][0], normals[n_idx][1], normals[n_idx][2]);

         // Set the vertex color
         glColor3f(vertices[v_idx].color[0], vertices[v_idx].color[1], vertices[v_idx].color[2]);

         // Set the vertex position
         glVertex3f(vertices[v_idx].position[0], vertices[v_idx].position[1], vertices[v_idx].position[2]);

          
         
      }

      glEnd();
      glPopMatrix();
   }
}

BodyFrame* readMotionCaptureData(const char* filename, int* frameCount) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open file: %s\n", filename);
        return NULL;
    }

    BodyFrame* frames = malloc(sizeof(BodyFrame) * MAX_FRAMES);
    if (!frames) {
        printf("Failed to allocate memory.\n");
        fclose(file);
        return NULL;
    }

    char line[1024]; // Increase line size to accommodate the new data
    int frameIndex = 0;
    int skipHeader = 1;

    while (fgets(line, sizeof(line), file)) {
        if (skipHeader) {
            skipHeader = 0;
            continue;
        }

        if (frameIndex >= MAX_FRAMES) {
            printf("Max frames limit reached.\n");
            break;
        }

        BodyFrame* frame = &frames[frameIndex];
        sscanf(line, "%*d," // Skip the frame number
               "%f,%f,%f,%f,%f,%f,"          // head and headRot
               "%f,%f,%f,%f,%f,%f,"          // leftShoulder and leftShoulderRot
               "%f,%f,%f,%f,%f,%f,"          // rightShoulder and rightShoulderRot
               "%f,%f,%f,%f,%f,%f,"          // leftHand and leftHandRot
               "%f,%f,%f,%f,%f,%f,"          // rightHand and rightHandRot
               "%f,%f,%f,%f,%f,%f,"          // leftElbow and leftElbowRot
               "%f,%f,%f,%f,%f,%f,"          // rightElbow and rightElbowRot
               "%f,%f,%f,%f,%f,%f,"          // leftFoot and leftFootRot
               "%f,%f,%f,%f,%f,%f,"          // rightFoot and rightFootRot
               "%f,%f,%f,%f,%f,%f,"          // leftKnee and leftKneeRot
               "%f,%f,%f,%f,%f,%f,"          // rightKnee and rightKneeRot
               "%f,%f,%f,%f,%f,%f,"          // leftHip and leftHipRot
               "%f,%f,%f,%f,%f,%f",          // rightHip and rightHipRot
               &frame->head.x, &frame->head.y, &frame->head.z,
               &frame->headRot.x, &frame->headRot.y, &frame->headRot.z,

               &frame->leftShoulder.x, &frame->leftShoulder.y, &frame->leftShoulder.z,
               &frame->leftShoulderRot.x, &frame->leftShoulderRot.y, &frame->leftShoulderRot.z,

               &frame->rightShoulder.x, &frame->rightShoulder.y, &frame->rightShoulder.z,
               &frame->rightShoulderRot.x, &frame->rightShoulderRot.y, &frame->rightShoulderRot.z,

               &frame->leftHand.x, &frame->leftHand.y, &frame->leftHand.z,
               &frame->leftHandRot.x, &frame->leftHandRot.y, &frame->leftHandRot.z,

               &frame->rightHand.x, &frame->rightHand.y, &frame->rightHand.z,
               &frame->rightHandRot.x, &frame->rightHandRot.y, &frame->rightHandRot.z,

               &frame->leftElbow.x, &frame->leftElbow.y, &frame->leftElbow.z,
               &frame->leftElbowRot.x, &frame->leftElbowRot.y, &frame->leftElbowRot.z,

               &frame->rightElbow.x, &frame->rightElbow.y, &frame->rightElbow.z,
               &frame->rightElbowRot.x, &frame->rightElbowRot.y, &frame->rightElbowRot.z,

               &frame->leftFoot.x, &frame->leftFoot.y, &frame->leftFoot.z,
               &frame->leftFootRot.x, &frame->leftFootRot.y, &frame->leftFootRot.z,

               &frame->rightFoot.x, &frame->rightFoot.y, &frame->rightFoot.z,
               &frame->rightFootRot.x, &frame->rightFootRot.y, &frame->rightFootRot.z,

               &frame->leftKnee.x, &frame->leftKnee.y, &frame->leftKnee.z,
               &frame->leftKneeRot.x, &frame->leftKneeRot.y, &frame->leftKneeRot.z,

               &frame->rightKnee.x, &frame->rightKnee.y, &frame->rightKnee.z,
               &frame->rightKneeRot.x, &frame->rightKneeRot.y, &frame->rightKneeRot.z,

               &frame->leftHip.x, &frame->leftHip.y, &frame->leftHip.z,
               &frame->leftHipRot.x, &frame->leftHipRot.y, &frame->leftHipRot.z,

               &frame->rightHip.x, &frame->rightHip.y, &frame->rightHip.z,
               &frame->rightHipRot.x, &frame->rightHipRot.y, &frame->rightHipRot.z);

        frameIndex++;
    }

    *frameCount = frameIndex;
    fclose(file);
    return frames;
}

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

// Check for OpenGL errors
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

// Print message to stderr and exit
void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}

//START of Sawyer Defined Objects ++++++++++++++++
void drawCubeAt(Vector3 pos, Vector3 rot){

   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   // if(WhichTexture != -1){
   //    glEnable(GL_TEXTURE_2D);
   //    // glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE); //old version that uses mode
   //    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); //just trying out the two modes.
   //    glBindTexture(GL_TEXTURE_2D,texture[WhichTexture]); 
   // }
   // glColor3f(1.0f, 0.184f, 0.0f); //red color for testing
   glPushMatrix();
      
   // glScaled(0.1f,0.1f,0.1f);
   glTranslated(pos.x,pos.y,pos.z);
   glScaled(0.3f, 0.3f, 0.3f); 

   // Apply rotations in Z-Y-X order
   glRotatef(rot.z, 0.0f, 0.0f, 1.0f);
   glRotatef(rot.y, 0.0f, 1.0f, 0.0f);
   glRotatef(rot.x, 1.0f, 0.0f, 0.0f); 

   glScaled(0.1f, 0.1f, 0.1f); 
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
   glPopMatrix(); 
   glDisable(GL_TEXTURE_2D);

}

void cubeHelper(double x, double y, double z){

   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   // if(WhichTexture != -1){
   //    glEnable(GL_TEXTURE_2D);
   //    // glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE); //old version that uses mode
   //    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); //just trying out the two modes.
   //    glBindTexture(GL_TEXTURE_2D,texture[WhichTexture]); 
   // }
   glTranslated(x,y,z); 
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

static void DrawFloor(double x, double y, double z, double SizeX, double SizeY, double SizeZ, int SplitCount) 
{
   glPushMatrix();
   glTranslated(x, y, z);
   glColor3f(0.9f, 0.9f, 0.9f); // Color for the floor
   glPushMatrix();
   glScaled(SizeX, SizeY, SizeZ);

   // Determine the size of each sub-section
   double step = 2.0 / SplitCount;

   glBegin(GL_QUADS);
   glNormal3f(0, 1, 0); // Set the normal facing upward

   // Create a grid of quads
   for (int i = 0; i < SplitCount; ++i) {
      for (int j = 0; j < SplitCount; ++j) {
         double x0 = -1.0 + i * step;
         double x1 = -1.0 + (i + 1) * step;
         double z0 = -1.0 + j * step;
         double z1 = -1.0 + (j + 1) * step;

         // Set texture coordinates and vertex positions for each smaller quad
         glTexCoord2f((float)i / SplitCount, (float)j / SplitCount); glVertex3f(x0, 0, z0); // Bottom-left
         glTexCoord2f((float)(i + 1) / SplitCount, (float)j / SplitCount); glVertex3f(x1, 0, z0); // Bottom-right
         glTexCoord2f((float)(i + 1) / SplitCount, (float)(j + 1) / SplitCount); glVertex3f(x1, 0, z1); // Top-right
         glTexCoord2f((float)i / SplitCount, (float)(j + 1) / SplitCount); glVertex3f(x0, 0, z1); // Top-left
      }
   }
   glEnd();

   glPopMatrix();
   glPopMatrix();
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
//END of Sawyer Defined Objects +++++++++++++

//HELPER FUNCTIONS
void playSound(const char* filename) {

    Mix_Chunk* sound = Mix_LoadWAV(filename);
    if (!sound) {
        printf("Failed to load sound! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }

    // Play the sound (channel -1 means any available channel)
    Mix_PlayChannel(-1, sound, 0);
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

//Vector math Helpers
// Function to calculate the distance between two points
float distanceOfPoints(Vector3 a, Vector3 b) {
    return sqrtf((b.x - a.x) * (b.x - a.x) + 
                 (b.y - a.y) * (b.y - a.y) + 
                 (b.z - a.z) * (b.z - a.z));
}

// Function to calculate a normalized vector
Vector3 normalize(Vector3 v) {
    float length = distanceOfPoints((Vector3){0, 0, 0}, v);
    return (Vector3){v.x / length, v.y / length, v.z / length};
}
Vector3 subtract(Vector3 a, Vector3 b) {
    return (Vector3){a.x - b.x, a.y - b.y, a.z - b.z};
}
float vectorLength(Vector3 v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
Vector3 scale(Vector3 v, float scalar) {
    return (Vector3){v.x * scalar, v.y * scalar, v.z * scalar};
}
Vector3 multiplyVec(Vector3 v1, Vector3 v2) {
    return (Vector3){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
}

//END OF HELPER FUNCTIONS



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
      glColor3ub(242, 140, 0);
      ball(Position[0],Position[1],Position[2] , 0.1);

      float Emission[]  = {0.0,0.0,0.0,1.0};
      // float Ambient[]   = {0.3,0.3,0.3,1.0};
      // float Diffuse[]   = {1.0,1.0,1.0,1.0};
      // float Specular[]  = {spc,spc,spc,1.0};
      float Shinyness[] = {16};
      //  OpenGL should normalize normal vectors 
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      //  Location of viewer for specular calculations
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
      //  glColor sets ambient and diffuse color materials
      //  Set materials
      
      // glColorMaterial(GL_FRONT_AND_BACK,GL_SPECULAR);
      // glColorMaterial(GL_FRONT_AND_BACK,GL_EMISSION);
      //  Set ambient, diffuse, specular components and position of light 0
   glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
   glLightfv(GL_LIGHT0,GL_POSITION,Position);
   //  Set materials
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,Shinyness);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,RGBA);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,RGBA);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Specular);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

  
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);

   }
   else{
      glDisable(GL_LIGHTING);
      
   }
   //SHAPES TO BE DRAWN SECTION **************

   //Drawing the OBJ model: 
   DrawObj(); 



   DrawFloor(0,-1.01,0,10,10,10,20); //lighting solved
   
   if(AnimatePerson)
   currentFrame = (int)(glutGet(GLUT_ELAPSED_TIME) / frameRateDelay) % frameCount;

   BodyFrame frame = motionData[currentFrame];
   
   glPushMatrix(); 
   glScaled(1.0f, 1.0f, 1.0f); 
   

   // HEAD
   glColor3f(0.0f, 0.8f, 0.0f);
   drawCubeAt(frame.head, frame.headRot);

   // SHOULDERS
   glColor3f(0.5f, 0.5f, 1.0f); // Blue color for shoulders
   drawCubeAt(frame.leftShoulder, frame.leftShoulderRot);
   drawCubeAt(frame.rightShoulder, frame.rightShoulderRot);

   // ELBOWS
   glColor3f(1.0f, 1.0f, 0.0f); // Yellow color for elbows
   drawCubeAt(frame.leftElbow, frame.leftElbowRot);
   drawCubeAt(frame.rightElbow, frame.rightElbowRot);

   // HANDS
   glColor3f(1.0f, 0.184f, 0.0f); // Orange color for hands
   drawCubeAt(frame.leftHand, frame.leftHandRot);
   drawCubeAt(frame.rightHand, frame.rightHandRot);

   // HIPS
   glColor3f(0.7f, 0.0f, 0.7f); // Purple color for hips
   drawCubeAt(frame.leftHip, frame.leftHipRot);
   drawCubeAt(frame.rightHip, frame.rightHipRot);

   // KNEES
   glColor3f(0.0f, 1.0f, 1.0f); // Cyan color for knees
   drawCubeAt(frame.leftKnee, frame.leftKneeRot);
   drawCubeAt(frame.rightKnee, frame.rightKneeRot);

   // FEET
   glColor3f(1.0f, 0.25f, 0.5f); // Pink color for feet
   drawCubeAt(frame.leftFoot, frame.leftFootRot);
   drawCubeAt(frame.rightFoot, frame.rightFootRot);



   // DRAW SKELETON

   //I think the trick here is goign to be that you call the draw arm function at the end of the shoulder function, before you pop the matrix


   glBegin(GL_LINES);
   // Left Upper Arm (Pink)
   glColor3f(1.0f, 0.0f, 0.5f); // Pink color
   glVertex3d(frame.leftShoulder.x, frame.leftShoulder.y, frame.leftShoulder.z);
   glVertex3d(frame.leftElbow.x, frame.leftElbow.y, frame.leftElbow.z);

   // Left Forearm (Pink)
   glVertex3d(frame.leftElbow.x, frame.leftElbow.y, frame.leftElbow.z);
   glVertex3d(frame.leftHand.x, frame.leftHand.y, frame.leftHand.z);

   // Right Upper Arm (Pink)
   glColor3f(1.0f, 0.0f, 0.5f); // Pink color
   glVertex3d(frame.rightShoulder.x, frame.rightShoulder.y, frame.rightShoulder.z);
   glVertex3d(frame.rightElbow.x, frame.rightElbow.y, frame.rightElbow.z);

   // Right Forearm (Pink)
   glVertex3d(frame.rightElbow.x, frame.rightElbow.y, frame.rightElbow.z);
   glVertex3d(frame.rightHand.x, frame.rightHand.y, frame.rightHand.z);

   // Left Upper Leg (Blue)
   glColor3f(0.0f, 0.0f, 1.0f); // Blue color
   glVertex3d(frame.leftHip.x, frame.leftHip.y, frame.leftHip.z);
   glVertex3d(frame.leftKnee.x, frame.leftKnee.y, frame.leftKnee.z);

   // Left Lower Leg (Blue)
   glVertex3d(frame.leftKnee.x, frame.leftKnee.y, frame.leftKnee.z);
   glVertex3d(frame.leftFoot.x, frame.leftFoot.y, frame.leftFoot.z);

   // Right Upper Leg (Blue)
   glColor3f(0.0f, 0.0f, 1.0f); // Blue color
   glVertex3d(frame.rightHip.x, frame.rightHip.y, frame.rightHip.z);
   glVertex3d(frame.rightKnee.x, frame.rightKnee.y, frame.rightKnee.z);

   // Right Lower Leg (Blue)
   glVertex3d(frame.rightKnee.x, frame.rightKnee.y, frame.rightKnee.z);
   glVertex3d(frame.rightFoot.x, frame.rightFoot.y, frame.rightFoot.z);

   // Torso (Green)
   glColor3f(0.0f, 1.0f, 0.0f); // Green color
   glVertex3d(frame.head.x, frame.head.y, frame.head.z);          // Neck/Head to left shoulder
   glVertex3d(frame.leftShoulder.x, frame.leftShoulder.y, frame.leftShoulder.z);

   glVertex3d(frame.head.x, frame.head.y, frame.head.z);          // Neck/Head to right shoulder
   glVertex3d(frame.rightShoulder.x, frame.rightShoulder.y, frame.rightShoulder.z);

   glVertex3d(frame.leftShoulder.x, frame.leftShoulder.y, frame.leftShoulder.z); // Left shoulder to left hip
   glVertex3d(frame.leftHip.x, frame.leftHip.y, frame.leftHip.z);

   glVertex3d(frame.rightShoulder.x, frame.rightShoulder.y, frame.rightShoulder.z); // Right shoulder to right hip
   glVertex3d(frame.rightHip.x, frame.rightHip.y, frame.rightHip.z);

   glVertex3d(frame.leftHip.x, frame.leftHip.y, frame.leftHip.z); // Connect left hip to right hip
   glVertex3d(frame.rightHip.x, frame.rightHip.y, frame.rightHip.z);

   glEnd();

   glPopMatrix(); 

   // DrawSkybox(10); 
   //END OF SHAPES TO BE DRAWN ***************
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1); //white
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
      // Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
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
   else if (ch == 'e'|| ch == 'E') {
      posY += 0.1; 
   }
   else if (ch == 'q' || ch == 'Q') {
      posY -= 0.1; 
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
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   //toggle light movement
   else if (ch == 'm' || ch == 'M')
      move = 1-move;
   //CONTROLLS FOR LIGHTING
   // //  Ambient level
   // else if (ch=='z' && ambient>0)
   //    ambient -= 5;
   // else if (ch=='Z' && ambient<100)
   //    ambient += 5;
   // //  Diffuse level
   // else if (ch=='x' && diffuse>0)
   //    diffuse -= 5;
   // else if (ch=='X' && diffuse<100)
   //    diffuse += 5;
   // //  Specular level
   // else if (ch=='c' && specular>0)
   //    specular -= 5;
   // else if (ch=='C' && specular<100)
   //    specular += 5;
   // //  Emission level
   // else if (ch=='v' && emission>0)
   //    emission -= 5;
   // else if (ch=='V' && emission<100)
   //    emission += 5;
   // //  Shininess level
   // else if (ch=='b' && shininess>-1)
   //    shininess -= 1;
   // else if (ch=='B' && shininess<7)
   //    shininess += 1;

   //  Toggle axes
   else if (ch == ' '){
      axes = 1-axes;
      AnimatePerson = 1-AnimatePerson; 
   }
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
      //   playSound("SuperhotMusic.mp3");
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
   glutCreateWindow("SUPERHOT 3RD Person BEEG");
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


   // motionData = readMotionCaptureData("../MoCapData/Wiggle.csv", &frameCount);
   // motionData = readMotionCaptureData("../MoCapData/RealWiggleAllPoints.csv", &frameCount); frameRateDelay = 2; 
   motionData = readMotionCaptureData("../MoCapData/BreakDancer60fps.csv", &frameCount); frameRateDelay = 18; 
   // motionData = readMotionCaptureData("../MoCapData/OrangeJustice.csv", &frameCount); frameRateDelay = 4; 
   // motionData = readMotionCaptureData("../MoCapData/HookAndJab.csv", &frameCount);

   if (argc >= 3) {
      ObjScale = atof(argv[2]);  // Convert to a double 
   }
   // if (argc>=6) RGBA[0] = strtod(argv[3],NULL);
   // if (argc>=6) RGBA[1] = strtod(argv[4],NULL);
   // if (argc>=6) RGBA[2] = strtod(argv[5],NULL);
   // SuperHotModel = LoadOBJ(argv[1]);
   loadOBJWithColor("SuperHotColored.obj");
   // printOBJData(); 



   if (!motionData) {
      return 1;
   }
   ErrCheck("init");

   glutMainLoop();
   
   free(motionData);
   Mix_CloseAudio();
   SDL_Quit();

   return 0;
}
