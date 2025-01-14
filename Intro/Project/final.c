//Don't forget to use gorod light smoothing for curves
#include "CSCIx229.h"
#include "supplemental.h"


const double worldSize = 1.8; 
const int waveResolution = 100; // Number of segments in the wave
float wavePositionZ = worldSize; // Initial z position of the wave
double t = 0; 

int move=1;       //  Move light
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
int ambient   =  50;  // Ambient intensity (%)
int diffuse   =  60;  // Diffuse intensity (%)
int specular  =   100;  // Specular intensity (%)
double spc=1;     //  Specular intensity
int shininess =   64;  // Shininess (power of two)
float shiny   =   32;  // Shininess (value)
int zh        = -90;  // Light azimuth
float ylight  = 0.8;  // Elevation of light
unsigned int texture[7]; // Texture names
unsigned int SkyboxTextures[6]; // Texture names
int textureIndex = 0;

int currentFrame = 0;
int frameRateDelay = 200;    //controlls playback speed for now
int AnimatePerson = 1; 
int frameCount = 0;
BodyFrame* motionData; 

int SuperHotModel;          //  Object display list
double ObjScale = 0.5; 

float RGBA[4] = {1,1,1,1};  //  Colors

int testModelTransform = 0; 

int whatToDraw = 2; 

typedef struct {
    int vertex[3];
    int normal;
} FaceDataIndex;

objVertex vertices[MAX_VERTICES];
float normals[MAX_NORMALS][3];
FaceDataIndex objDataIndices[MAX_NORMALS]; 

objVertex verticesCOPY[MAX_VERTICES];
float normalsCOPY[MAX_NORMALS][3];
FaceDataIndex objDataIndicesCOPY[MAX_NORMALS]; 

Vector3 bodyPartRotatePoints[14];

Vector3 BoneManBoneVectors[10]; 
Vector3 MeshManBoneVectors[10]; 

Matrix4 BoneTransformationMatricies[10]; 
Matrix4 bodypartTransformationMatricesForJoints[10];


int vertex_count = 0;
int normal_count = 0;
int face_count = 0; 

int shader[]  = {0,0}; //  Shader programs

void populateRotatePoints(){
   Vector3 headRotPos; headRotPos.x = -0.0339; headRotPos.y = 3.5; headRotPos.z = 0.4728; 
   bodyPartRotatePoints[0] = headRotPos; 

   //left arm
   Vector3 LeftUpperArmRotPos; LeftUpperArmRotPos.x = 0.85115; LeftUpperArmRotPos.y = 2.8044; LeftUpperArmRotPos.z = 0.29265; 
   bodyPartRotatePoints[2] = LeftUpperArmRotPos;
   Vector3 LeftLowerArmRotPos; LeftLowerArmRotPos.x = 1.07624; LeftLowerArmRotPos.y = 1.91877; LeftLowerArmRotPos.z = 0.329963; 
   bodyPartRotatePoints[3] = LeftLowerArmRotPos;
   //right arm
   Vector3 RightUpperArmRotPos; RightUpperArmRotPos.x = -0.71292; RightUpperArmRotPos.y = 2.96523; RightUpperArmRotPos.z = 0.497993; 
   bodyPartRotatePoints[4] = RightUpperArmRotPos;
   Vector3 RightLowerArmRotPos; RightLowerArmRotPos.x = -1.02099; RightLowerArmRotPos.y = 2.08705; RightLowerArmRotPos.z = 0.502834; 
   bodyPartRotatePoints[5] = RightLowerArmRotPos;
   //Left Leg
   Vector3 LeftLegUpperRotPos; LeftLegUpperRotPos.x = 0.245658 ; LeftLegUpperRotPos.y = 0.631263; LeftLegUpperRotPos.z = 0.984649; 
   bodyPartRotatePoints[6] = LeftLegUpperRotPos;
   Vector3 LeftLegLowerRotPos; LeftLegLowerRotPos.x = 0.392758; LeftLegLowerRotPos.y = -1.11073 ; LeftLegLowerRotPos.z = 1.19662; 
   bodyPartRotatePoints[7] = LeftLegLowerRotPos;
   //right Leg
   Vector3 RightLegUpperRotPos; RightLegUpperRotPos.x = -0.332425; RightLegUpperRotPos.y = 0.71882; RightLegUpperRotPos.z = 0.913496; 
   bodyPartRotatePoints[8] = RightLegUpperRotPos;
   Vector3 RightLegLowerRotPos; RightLegLowerRotPos.x = -0.443468; RightLegLowerRotPos.y = -1.28511; RightLegLowerRotPos.z = 1.2823; 
   bodyPartRotatePoints[9] = RightLegLowerRotPos;

   bodyPartRotatePoints[1] = averageVector3(LeftLegUpperRotPos, RightLegUpperRotPos);

   Vector3 LeftHandRotPos; LeftHandRotPos.x = 0.372929; LeftHandRotPos.y = -3.1079; LeftHandRotPos.z = 1.36005;
   Vector3 RightHandRotPos; RightHandRotPos.x = -0.415594; RightHandRotPos.y = -3.14632; RightHandRotPos.z = 1.40471; 
   Vector3 LeftFootRotPos; LeftFootRotPos.x = 1.12562; LeftFootRotPos.y = 0.835937; LeftFootRotPos.z = 0.575556; 
   Vector3 RightFootRotPos; RightFootRotPos.x = -1.08507; RightFootRotPos.y = 0.887655; RightFootRotPos.z = 0.826123; 

   bodyPartRotatePoints[10] = LeftHandRotPos; 
   bodyPartRotatePoints[11] = RightHandRotPos;
   bodyPartRotatePoints[12] = LeftFootRotPos; 
   bodyPartRotatePoints[13] = RightFootRotPos;
}

void populateBoneTransformationMatricies(BodyFrame frame) {
   //chest
   Vector3 BoneManBoneVector_chest = subtract(averageVector3(frame.leftShoulder, frame.rightShoulder), averageVector3(frame.leftHip, frame.rightHip));
   Vector3 MeshManBoneVector_chest = subtract(bodyPartRotatePoints[0], bodyPartRotatePoints[1]);
   BoneTransformationMatricies[1] = transformVectorToVector(MeshManBoneVector_chest, BoneManBoneVector_chest);
   // Left Arm
   Vector3 BoneManBoneVector_LeftUpperArm = subtract(frame.leftShoulder, frame.leftElbow);
   Vector3 MeshManBoneVector_LeftUpperArm = subtract(bodyPartRotatePoints[2], bodyPartRotatePoints[3]);
   BoneTransformationMatricies[2] = transformVectorToVector(MeshManBoneVector_LeftUpperArm, BoneManBoneVector_LeftUpperArm);

   Vector3 BoneManBoneVector_LeftLowerArm = subtract(frame.leftElbow, frame.leftHand);
   Vector3 MeshManBoneVector_LeftLowerArm = subtract(bodyPartRotatePoints[3], bodyPartRotatePoints[10]);
   BoneTransformationMatricies[3] = transformVectorToVector(MeshManBoneVector_LeftLowerArm, BoneManBoneVector_LeftLowerArm);

   // Right Arm
   Vector3 BoneManBoneVector_RightUpperArm = subtract(frame.rightShoulder, frame.rightElbow);
   Vector3 MeshManBoneVector_RightUpperArm = subtract(bodyPartRotatePoints[4], bodyPartRotatePoints[5]);
   BoneTransformationMatricies[4] = transformVectorToVector(MeshManBoneVector_RightUpperArm, BoneManBoneVector_RightUpperArm);

   Vector3 BoneManBoneVector_RightLowerArm = subtract(frame.rightElbow, frame.rightHand);
   Vector3 MeshManBoneVector_RightLowerArm = subtract(bodyPartRotatePoints[5], bodyPartRotatePoints[11]);
   BoneTransformationMatricies[5] = transformVectorToVector(MeshManBoneVector_RightLowerArm, BoneManBoneVector_RightLowerArm);

   // Left Leg
   Vector3 BoneManBoneVector_LeftUpperLeg = subtract(frame.leftHip, frame.leftKnee);
   Vector3 MeshManBoneVector_LeftUpperLeg = subtract(bodyPartRotatePoints[6], bodyPartRotatePoints[7]);
   BoneTransformationMatricies[6] = transformVectorToVector(MeshManBoneVector_LeftUpperLeg, BoneManBoneVector_LeftUpperLeg);

   Vector3 BoneManBoneVector_LeftLowerLeg = subtract(frame.leftFoot, frame.leftKnee);
   Vector3 MeshManBoneVector_LeftLowerLeg = subtract(bodyPartRotatePoints[7], bodyPartRotatePoints[12]);
   BoneTransformationMatricies[7] = transformVectorToVector(MeshManBoneVector_LeftLowerLeg, BoneManBoneVector_LeftLowerLeg);

   // Right Leg
   Vector3 BoneManBoneVector_RightUpperLeg = subtract(frame.rightHip, frame.rightKnee);
   Vector3 MeshManBoneVector_RightUpperLeg = subtract(bodyPartRotatePoints[8], bodyPartRotatePoints[9]);
   BoneTransformationMatricies[8] = transformVectorToVector(MeshManBoneVector_RightUpperLeg, BoneManBoneVector_RightUpperLeg);

   Vector3 BoneManBoneVector_RightLowerLeg = subtract(frame.rightFoot, frame.rightKnee);
   Vector3 MeshManBoneVector_RightLowerLeg = subtract(bodyPartRotatePoints[9], bodyPartRotatePoints[13]);
   BoneTransformationMatricies[9] = transformVectorToVector(MeshManBoneVector_RightLowerLeg, BoneManBoneVector_RightLowerLeg);
}

void copyObjData() {
    // Copy vertices
    for (int i = 0; i < vertex_count; i++) {
        verticesCOPY[i] = vertices[i];
    }

    // Copy normals
    for (int i = 0; i < normal_count; i++) {
        for (int j = 0; j < 3; j++) {
            normalsCOPY[i][j] = normals[i][j];
        }
    }

    // Copy face data indices
    for (int i = 0; i < face_count; i++) {
        objDataIndicesCOPY[i] = objDataIndices[i];
    }
}

void applyMatrixToBodyPartCopyPoints(Matrix4* transformationMatrix, int bodyPartId) {
    for (int i = 0; i < vertex_count; i++) {
        // Check if the vertex belongs to the body part
        if (MatchesBodyPartColor(vertices[i].color) == bodyPartId) {
            // Transform the vertex position
            Vector3 originalPosition = {
                vertices[i].position[0],
                vertices[i].position[1],
                vertices[i].position[2]
            };

            Vector3 transformedPosition = multiplyMatrixVector(transformationMatrix, &originalPosition);

            // Update the position in the COPY
            verticesCOPY[i].position[0] = transformedPosition.x;
            verticesCOPY[i].position[1] = transformedPosition.y;
            verticesCOPY[i].position[2] = transformedPosition.z;

            // Transform the normal
            Vector3 originalNormal = {
                normals[objDataIndices[i].normal - 1][0],
                normals[objDataIndices[i].normal - 1][1],
                normals[objDataIndices[i].normal - 1][2]
            };

            Vector3 transformedNormal = multiplyMatrixVector(transformationMatrix, &originalNormal);
            // Normalize the transformed normal
            transformedNormal = normalize(transformedNormal);

            // Update the normal in the COPY
            normalsCOPY[objDataIndices[i].normal - 1][0] = transformedNormal.x;
            normalsCOPY[objDataIndices[i].normal - 1][1] = transformedNormal.y;
            normalsCOPY[objDataIndices[i].normal - 1][2] = transformedNormal.z;
        }
    }
}


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
    
}
    }
     

    fclose(file);
}


void DrawObj() {
   // Transformation for left upper arm vertices (color 0.2, 0, 1)
   for (int face = 0; face < face_count; face++) {
      glPushMatrix();
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
      
         glNormal3f(normals[n_idx][0], normals[n_idx][1], normals[n_idx][2]);
         glColor3f(vertices[v_idx].color[0], vertices[v_idx].color[1], vertices[v_idx].color[2]);
         glVertex3f(vertices[v_idx].position[0], vertices[v_idx].position[1], vertices[v_idx].position[2]); 
         
         
      }

      glEnd();
      glPopMatrix();
      
   }

   testModelTransform = 1;
}



void Drawbodypart(int bodypartid) {
   // Transformation for left upper arm vertices (color 0.2, 0, 1)
   // float transform[3] = {0.05f, 0.05f, 0.05f}; // Translation vector
   for (int face = 0; face < face_count; face++) {
      glPushMatrix();
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

         // Check if this vertex belongs to a certain bodypart based on color
         if(MatchesBodyPartColor(vertices[v_idx].color) == bodypartid){

            glNormal3f(normals[n_idx][0], normals[n_idx][1], normals[n_idx][2]);
            glColor3f(vertices[v_idx].color[0], vertices[v_idx].color[1], vertices[v_idx].color[2]);
            // glColor3f(0.9f, 0.2f, 0.2f);

            glVertex3f(vertices[v_idx].position[0], vertices[v_idx].position[1], vertices[v_idx].position[2]); 
         }
         
      }

      glEnd();
      glPopMatrix();
      glColor3f(0.9f, 0.9f, 0.9f);
   }
}


void calculateAndDrawNormals() {
   glColor3f(0.1f, 0.8f, 0.8f); //blue color for testing

    for (int face = 0; face < face_count; face++) {
        // Get the vertices of the triangle
        int v1_idx = objDataIndicesCOPY[face].vertex[0] - 1;
        int v2_idx = objDataIndicesCOPY[face].vertex[1] - 1;
        int v3_idx = objDataIndicesCOPY[face].vertex[2] - 1;

        // Retrieve positions of the vertices
        float v1[3] = {verticesCOPY[v1_idx].position[0],
                       verticesCOPY[v1_idx].position[1],
                       verticesCOPY[v1_idx].position[2]};
        float v2[3] = {verticesCOPY[v2_idx].position[0],
                       verticesCOPY[v2_idx].position[1],
                       verticesCOPY[v2_idx].position[2]};
        float v3[3] = {verticesCOPY[v3_idx].position[0],
                       verticesCOPY[v3_idx].position[1],
                       verticesCOPY[v3_idx].position[2]};

        // Calculate edge vectors
        float edge1[3] = {v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]};
        float edge2[3] = {v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2]};

        // Calculate the normal using the cross product
        float normal[3] = {
            edge1[1] * edge2[2] - edge1[2] * edge2[1],
            edge1[2] * edge2[0] - edge1[0] * edge2[2],
            edge1[0] * edge2[1] - edge1[1] * edge2[0]};

        // Normalize the normal vector
        float length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
        if (length != 0.0f) {
            normal[0] /= length;
            normal[1] /= length;
            normal[2] /= length;
        }

        // Calculate the centroid of the triangle
      //   float centroid[3] = {
      //       (v1[0] + v2[0] + v3[0]) / 3.0f,
      //       (v1[1] + v2[1] + v3[1]) / 3.0f,
      //       (v1[2] + v2[2] + v3[2]) / 3.0f};

        // Calculate the endpoint of the normal line
      //   float endpoint[3] = {
      //       centroid[0] + normal[0]/10,
      //       centroid[1] + normal[1]/10,
      //       centroid[2] + normal[2]/10};

        // Draw the triangle
         glColor3f(0.1f, 0.1f, 0.1f); //red color for testing

        glBegin(GL_TRIANGLES);

        // First vertex
        glNormal3f(normal[0], normal[1], normal[2]);
        glVertex3f(v1[0], v1[1], v1[2]);

        // Second vertex
        glNormal3f(normal[0], normal[1], normal[2]);
        glVertex3f(v2[0], v2[1], v2[2]);

        // Third vertex
        glNormal3f(normal[0], normal[1], normal[2]);
        glVertex3f(v3[0], v3[1], v3[2]);

        glEnd();

        // For drawing the normal lines
      //   glBegin(GL_LINES);

      //   glVertex3f(centroid[0], centroid[1], centroid[2]);
      //   glVertex3f(endpoint[0], endpoint[1], endpoint[2]);

      //   glEnd();

    }

}

void fillInJoints() {
    for (int face = 0; face < face_count; face++) {
        int bodyPartIds[3];
      //   int verticesInDifferentParts = 0;

        // Determine the body parts for each vertex in the face
        for (int i = 0; i < 3; i++) {
            int v_idx = objDataIndicesCOPY[face].vertex[i] - 1; // Adjust for 1-based indexing
            bodyPartIds[i] = MatchesBodyPartColor(verticesCOPY[v_idx].color); // Use 0 to get body part ID
            if (bodyPartIds[i] != bodyPartIds[0]) {
               //  verticesInDifferentParts = 1; // At least one vertex is in a different body part
            }
        }

        // If all vertices are in the same body part, skip
      //   if (!verticesInDifferentParts) {
      //       continue;
      //   }

        // Create new triangles to fill gaps between body parts
        for (int i = 0; i < 3; i++) {
            for (int j = i + 1; j < 3; j++) {
               //  if (bodyPartIds[i] != bodyPartIds[j]) {
                    // Create a triangle between two vertices from different body parts
                    glBegin(GL_TRIANGLES);
                    // First vertex
                  //   glNormal3f(normalsCOPY[objDataIndicesCOPY[face].normal - 1][0],
                  //              normalsCOPY[objDataIndicesCOPY[face].normal - 1][1],
                  //              normalsCOPY[objDataIndicesCOPY[face].normal - 1][2]);
                    glVertex3f(verticesCOPY[objDataIndicesCOPY[face].vertex[i] - 1].position[0],
                               verticesCOPY[objDataIndicesCOPY[face].vertex[i] - 1].position[1],
                               verticesCOPY[objDataIndicesCOPY[face].vertex[i] - 1].position[2]);

                    // Second vertex
                  //   glNormal3f(normalsCOPY[objDataIndicesCOPY[face].normal - 1][0],
                  //              normalsCOPY[objDataIndicesCOPY[face].normal - 1][1],
                  //              normalsCOPY[objDataIndicesCOPY[face].normal - 1][2]);
                    glVertex3f(verticesCOPY[objDataIndicesCOPY[face].vertex[j] - 1].position[0],
                               verticesCOPY[objDataIndicesCOPY[face].vertex[j] - 1].position[1],
                               verticesCOPY[objDataIndicesCOPY[face].vertex[j] - 1].position[2]);

                    // Third vertex - this closes the triangle by using the remaining vertex in the face
                    int k = 3 - i - j; // The index of the third vertex
                  //   glNormal3f(normalsCOPY[objDataIndicesCOPY[face].normal - 1][0],
                  //              normalsCOPY[objDataIndicesCOPY[face].normal - 1][1],
                  //              normalsCOPY[objDataIndicesCOPY[face].normal - 1][2]);
                    glVertex3f(verticesCOPY[objDataIndicesCOPY[face].vertex[k] - 1].position[0],
                               verticesCOPY[objDataIndicesCOPY[face].vertex[k] - 1].position[1],
                               verticesCOPY[objDataIndicesCOPY[face].vertex[k] - 1].position[2]);

                    glEnd();
                }
               // Calculate the centroid of the triangle
               // glColor3f(0.1f, 0.8f, 0.8f); //blue color for testing

               // float centroid[3] = {0.0f, 0.0f, 0.0f};
               // for (int m = 0; m < 3; m++) {
               //    centroid[0] += verticesCOPY[objDataIndicesCOPY[face].vertex[m] - 1].position[0];
               //    centroid[1] += verticesCOPY[objDataIndicesCOPY[face].vertex[m] - 1].position[1];
               //    centroid[2] += verticesCOPY[objDataIndicesCOPY[face].vertex[m] - 1].position[2];
               // }
               // centroid[0] /= 3.0f;
               // centroid[1] /= 3.0f;
               // centroid[2] /= 3.0f;

               // // Get the normal vector
               // float normal[3] = {
               //    normalsCOPY[objDataIndicesCOPY[face].normal - 1][0],
               //    normalsCOPY[objDataIndicesCOPY[face].normal - 1][1],
               //    normalsCOPY[objDataIndicesCOPY[face].normal - 1][2]
               // };

               // // Calculate the endpoint of the line
               // float endpoint[3] = {
               //    centroid[0] + normal[0]/10,
               //    centroid[1] + normal[1]/10,
               //    centroid[2] + normal[2]/10
               // };

               // // Draw the normal as a line
               // glBegin(GL_LINES);
               // // Line starts at the centroid
               // glVertex3f(centroid[0], centroid[1], centroid[2]);
               // // Line ends at the centroid plus the normal
               // glVertex3f(endpoint[0], endpoint[1], endpoint[2]);
               // glEnd();
               glColor3f(1.0f, 0.2f, 0.1f); //red color for testing

            // }
        }
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
               "%f,%f,%f,%f,%f,%f,"          //overall movement
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

               &frame->overall.x, &frame->overall.y, &frame->overall.z,
               &frame->overallRot.x, &frame->overallRot.y, &frame->overallRot.z,

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
   //    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); //just trying out the two modes.
   //    glBindTexture(GL_TEXTURE_2D,texture[WhichTexture]); 
   // }
   // glColor3f(1.0f, 0.184f, 0.0f); //red color for testing
   glPushMatrix();
      
   // glScaled(0.1f,0.1f,0.1f);
   glTranslated(pos.x,pos.y,pos.z);
   // glScaled(0.3f, 0.3f, 0.3f); 

   // Apply rotations in Z-Y-X order
   glRotatef(rot.z, 0.0f, 0.0f, 1.0f);
   glRotatef(rot.y, 0.0f, 1.0f, 0.0f);
   glRotatef(rot.x, 1.0f, 0.0f, 0.0f); 

   glScaled(0.01f, 0.01f, 0.01f); 
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

//END of Sawyer Defined Objects +++++++++++++

//HELPER FUNCTIONS
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
//END OF HELPER FUNCTIONS
Matrix4 ComputeBodyPartTransformMatrix(int bodyPartIndex, Vector3 position, Vector3 rotation, Matrix4* matrix) {
    // Initialize the transformation matrix as an identity matrix
    Matrix4 transformMatrix = identityMatrix();

    // Translation to the body part position
    Matrix4 translationMatrix = createTranslationMatrix(position.x, position.y, position.z);
    transformMatrix = multiplyMatrices(&transformMatrix, &translationMatrix);

    // Apply rotations
    if (bodyPartIndex == 1) {
        // Rotations for the head (body part index 1)
      Matrix4 rotationX = createRotationMatrixX(rotation.x);
      Matrix4 rotationY = createRotationMatrixY(rotation.y);
      Matrix4 rotationZ = createRotationMatrixZ(rotation.z);
      transformMatrix = multiplyMatrices(&transformMatrix, &rotationZ);
      transformMatrix = multiplyMatrices(&transformMatrix, &rotationY);
      transformMatrix = multiplyMatrices(&transformMatrix, &rotationX);
       
        
    } else {
        // Apply the special matrix for body parts other than the head
        transformMatrix = multiplyMatrices(&transformMatrix, matrix);
    }

    // Apply scaling
    Matrix4 scaleMatrix;
    if (bodyPartIndex == 2) {
        // Special scaling for the torso
        scaleMatrix = createScaleMatrix(0.24f, 0.24f, 0.24f);
    } else {
        // Default scaling for other body parts
        scaleMatrix = createScaleMatrix(0.30f, 0.30f, 0.30f);
    }
    transformMatrix = multiplyMatrices(&transformMatrix, &scaleMatrix);

    // Translate back to the rotation origin of the body part
    Vector3 negateRotationPoint = negateVector3(bodyPartRotatePoints[bodyPartIndex - 1]);
    Matrix4 backTranslationMatrix = createTranslationMatrix(negateRotationPoint.x, negateRotationPoint.y, negateRotationPoint.z);
    transformMatrix = multiplyMatrices(&transformMatrix, &backTranslationMatrix);

    return transformMatrix;
}




void DrawBodyPartWithTransform(int bodyPartIndex, Vector3 position, Vector3 rotation, Matrix4* matrix) {
   glPushMatrix();
   // Translate to the position of the body part
   glTranslated(position.x, position.y, position.z);
   // Apply rotations
    
   if(bodyPartIndex == 1){
      glRotated(rotation.x, 1.0, 0.0, 0.0);
      glRotated(rotation.y, 0.0, 1.0, 0.0);
      glRotated(rotation.z, 0.0, 0.0, 1.0);
   }
   else{
      glMultMatrixf(matrix->m);
   }
   if(bodyPartIndex == 2){
      glScaled(0.24f, 0.24f, 0.24f);
   }
   else{
      glScaled(0.30f, 0.30f, 0.30f);
   }

   glTranslated(-bodyPartRotatePoints[bodyPartIndex-1].x, -bodyPartRotatePoints[bodyPartIndex-1].y, -bodyPartRotatePoints[bodyPartIndex-1].z); //translate back to origin
   
   // Draw the body part
   bodypartTransformationMatricesForJoints[bodyPartIndex - 1] = ComputeBodyPartTransformMatrix(bodyPartIndex, position, rotation, matrix); 
   if(whatToDraw == 1)
      Drawbodypart(bodyPartIndex);
   glPopMatrix();
}

//this is for the stencil shader
void DrawFullscreenQuad() {
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); //just trying out the two modes.
   glBindTexture(GL_TEXTURE_2D,texture[textureIndex]); 

   // Switch to orthographic projection
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

   // Switch to modelview matrix
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   // Draw the fullscreen quad in NDC
   glRotated(1,0,0,GLUT_ELAPSED_TIME); 
   glBegin(GL_QUADS);

   if(textureIndex == 0){
      glColor4f(1.0f,1.0f,1.0f,0.05f); 

      glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f); // Bottom-left
      glTexCoord2f(8.0f, 0.0f); glVertex2f(1.0f, -1.0f);  // Bottom-right
      glTexCoord2f(8.0f, 8.0f); glVertex2f(1.0f, 1.0f);   // Top-right
      glTexCoord2f(0.0f, 8.0f); glVertex2f(-1.0f, 1.0f);  // Top-left
   }
   else{
      glColor4f(1.0f,1.0f,1.0f,0.5f); 

      glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f); // Bottom-left
      glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);  // Bottom-right
      glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);   // Top-right
      glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);  // Top-left
   }
   
   glEnd();

   // Restore the previous projection and modelview matrices
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);

   glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);


}

void display()
{

   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //for the lines shader
   glEnable(GL_STENCIL_TEST);

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
      float Specular[]  = {255/100,140/100,0,1.0};
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

   if(AnimatePerson)
   currentFrame = (int)(glutGet(GLUT_ELAPSED_TIME) / frameRateDelay) % frameCount;

   BodyFrame frame = motionData[currentFrame];

   //these are the cubes for Boneman
   if(whatToDraw == 3){
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
      // I think the trick here is goign to be that you call the draw arm function at the end of the shoulder function, before you pop the matrix

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
   }
   
   populateBoneTransformationMatricies(frame); 
    // Head
   glPushMatrix();
   Matrix4 identity = identityMatrix(); 
   DrawBodyPartWithTransform(1, frame.head, frame.headRot, &identity);
   DrawBodyPartWithTransform(2, averageVector3(frame.leftHip, frame.rightHip), frame.headRot, &BoneTransformationMatricies[1]);

   DrawBodyPartWithTransform(3, frame.leftShoulder, frame.leftShoulderRot, &BoneTransformationMatricies[2]);
   DrawBodyPartWithTransform(4, frame.leftElbow, frame.leftElbowRot, &BoneTransformationMatricies[3]);

   DrawBodyPartWithTransform(5, frame.rightShoulder, frame.rightShoulderRot, &BoneTransformationMatricies[4]);
   DrawBodyPartWithTransform(6, frame.rightElbow, frame.rightElbowRot, &BoneTransformationMatricies[5]);

   DrawBodyPartWithTransform(7, frame.leftHip, frame.leftHipRot, &BoneTransformationMatricies[6]);
   DrawBodyPartWithTransform(8, frame.leftKnee, frame.leftKneeRot, &BoneTransformationMatricies[7]);

   DrawBodyPartWithTransform(9, frame.rightHip, frame.rightHipRot, &BoneTransformationMatricies[8]);
   DrawBodyPartWithTransform(10, frame.rightKnee, frame.rightKneeRot, &BoneTransformationMatricies[9]);

   for(int i = 0; i <= 10; i++){
      applyMatrixToBodyPartCopyPoints(&bodypartTransformationMatricesForJoints[i-1], i);
   }
   glColor3f(0.9f, 0.2f, 0.2f);
   if(whatToDraw == 2){
      // Step 1: Render the portal objects into the stencil buffer
      glStencilFunc(GL_ALWAYS, 1, 0xFF); // Always pass
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // Replace stencil value with 1 where drawn
      glStencilMask(0xFF); // Write to the stencil buffer
      // glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable color writing
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

      calculateAndDrawNormals(); 
      fillInJoints();

      // Step 2: Render the portal effect (fullscreen quad)
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Enable color writing
      glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass only where stencil buffer is 1
      glStencilMask(0x00); // Don't modify the stencil buffer
      glDisable(GL_DEPTH_TEST); // Disable depth test for the fullscreen quad

      // Use a shader for the portal effect
      // glUseProgram(0); // Activate your shader program
      DrawFullscreenQuad(); // Render the fullscreen quad with the portal effect
      // glUseProgram(0); // Deactivate shaders
      glEnable(GL_DEPTH_TEST); // Re-enable depth testing

      // Step 3: Render the rest of the scene
      glStencilFunc(GL_ALWAYS, 0, 0xFF); // Clear the stencil buffer effect
      glStencilMask(0xFF); // Allow stencil buffer modifications
      glDisable(GL_STENCIL_TEST); // Disable stencil testing for the rest of the scene
   }
   glColor3f(0.9f, 0.9f, 0.9f);
   glPopMatrix();

   DrawFloor(0,-1.01,0,20,20,20,20); //lighting solved
   DrawSkybox(25); 
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
   else if (ch == 't' || ch == 'T') {
      textureIndex = (textureIndex+1)%2; 
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
   else if(ch == '1')
      whatToDraw = 1; 
   else if(ch == '2')
      whatToDraw = 2;
   else if(ch == '3')
      whatToDraw = 3;
   else if(ch == '4'){
      motionData = readMotionCaptureData("MoCapData/BreakDancer60fps.csv", &frameCount); frameRateDelay = 20; 
   }
   else if(ch == '5'){
      motionData = readMotionCaptureData("MoCapData/BackFlip.csv", &frameCount); frameRateDelay = 20; 
   }
   else if(ch == '6'){
      motionData = readMotionCaptureData("MoCapData/HookJab60fps.csv", &frameCount); frameRateDelay = 20; 
   }

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
   glutCreateWindow("Sawyer Reinig SUPERHOT 3RD Person");
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
   texture[0] = LoadTexBMP("SlantedLines.bmp");
   texture[1] = LoadTexBMP("Nebula2.bmp");

   
   motionData = readMotionCaptureData("MoCapData/HookJab60fps.csv", &frameCount); frameRateDelay = 20; 

   if (argc >= 3) { 
      ObjScale = atof(argv[2]);  // Convert to a double 
   }
   
   loadOBJWithColor("Objs/SuperHotColored.obj");
   copyObjData(); 
   populateRotatePoints(); 

   


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
