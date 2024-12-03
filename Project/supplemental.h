#ifndef SUPPLEMENTAL_H
#define SUPPLEMENTAL_H
#include "CSCIx229.h"


// Function declarations
void supplementalFunction();

float distanceOfPoints(Vector3 a, Vector3 b); 
Vector3 normalize(Vector3 v); 
Vector3 subtract(Vector3 a, Vector3 b); 
float vectorLength(Vector3 v); 
Vector3 scale(Vector3 v, float scalar); 
Vector3 multiplyVec(Vector3 v1, Vector3 v2); 
float dot(Vector3 a, Vector3 b); 
Vector3 cross(Vector3 a, Vector3 b); 
void printMatrix(const Matrix4* mat); 
Matrix4 axisAngleMatrix(Vector3 axis, float angle); 
Matrix4 transformVectorToVector(Vector3 A, Vector3 B); 
Vector3 multiplyMatrixVector(const Matrix4* matrix, const Vector3* vector); 
void printVector3(Vector3 vector); 
Matrix4 identityMatrix();
Vector3 averageVector3(Vector3 v1, Vector3 v2); 

Matrix4 createTranslationMatrix(float tx, float ty, float tz); 
Matrix4 createScaleMatrix(float sx, float sy, float sz); 
Matrix4 createRotationMatrixX(float angle); 
Matrix4 createRotationMatrixY(float angle); 
Matrix4 createRotationMatrixZ(float angle); 
Matrix4 multiplyMatrices(const Matrix4* a, const Matrix4* b); 
Vector3 negateVector3(Vector3 v); 


void playSound(const char* filename); 
int initAudio(); 
int MatchesBodyPartColor(float color[3]); 




#endif
