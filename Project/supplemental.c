#include "supplemental.h"
#include "CSCIx229.h"
#include <stdio.h>

void supplementalFunction() {
    printf("Hello from supplemental.c!\n");
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
float dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
Vector3 cross(Vector3 a, Vector3 b) {
    Vector3 result = {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    };
    return result;
}

Vector3 averageVector3(Vector3 v1, Vector3 v2) {
    Vector3 result;
    result.x = (v1.x + v2.x) / 2.0f;
    result.y = (v1.y + v2.y) / 2.0f;
    result.z = (v1.z + v2.z) / 2.0f;
    return result;
}

Matrix4 identityMatrix() {
    Matrix4 mat;
    memset(mat.m, 0, sizeof(mat.m));
    mat.m[0] = mat.m[5] = mat.m[10] = mat.m[15] = 1.0f;
    return mat;
}

void printMatrix(const Matrix4* mat) {
    for (int i = 0; i < 4; i++) {
        printf("%f %f %f %f\n",
            mat->m[i], mat->m[i + 4], mat->m[i + 8], mat->m[i + 12]);
    }
}
void printVector3(Vector3 vector){
        printf("[%f, %f, %f]\n", vector.x, vector.y, vector.z); 
}

Matrix4 transformVectorToVector(Vector3 v1, Vector3 v2) {
    Vector3 from = normalize(v1);
    Vector3 to = normalize(v2);

    // Compute the rotation axis as the cross product
    Vector3 axis = cross(from, to);
    float angle = acos(dot(from, to));

    // If the vectors are parallel (angle == 0), return identity matrix
    if (fabs(angle) < 1e-6) {
        Matrix4 identity = {.m = {1, 0, 0, 0,
                                  0, 1, 0, 0,
                                  0, 0, 1, 0,
                                  0, 0, 0, 1}};
        return identity;
    }

    // Normalize the axis
    axis = normalize(axis);

    // Compute the rotation matrix using Rodrigues' formula
    float c = cos(angle);
    float s = sin(angle);
    float t = 1 - c;

    Matrix4 rotation;
    rotation.m[0] = t * axis.x * axis.x + c;
    rotation.m[1] = t * axis.x * axis.y + s * axis.z;
    rotation.m[2] = t * axis.x * axis.z - s * axis.y;
    rotation.m[3] = 0;

    rotation.m[4] = t * axis.x * axis.y - s * axis.z;
    rotation.m[5] = t * axis.y * axis.y + c;
    rotation.m[6] = t * axis.y * axis.z + s * axis.x;
    rotation.m[7] = 0;

    rotation.m[8] = t * axis.x * axis.z + s * axis.y;
    rotation.m[9] = t * axis.y * axis.z - s * axis.x;
    rotation.m[10] = t * axis.z * axis.z + c;
    rotation.m[11] = 0;

    rotation.m[12] = 0;
    rotation.m[13] = 0;
    rotation.m[14] = 0;
    rotation.m[15] = 1;

    return rotation;
}

Vector3 multiplyMatrixVector(const Matrix4* matrix, const Vector3* vector) {
    Vector3 result;

    // Treat vector as a 4D vector with w=1
    float x = vector->x;
    float y = vector->y;
    float z = vector->z;
    float w = 1.0f;

    // Perform the matrix multiplication
    result.x = matrix->m[0] * x + matrix->m[4] * y + matrix->m[8]  * z + matrix->m[12] * w;
    result.y = matrix->m[1] * x + matrix->m[5] * y + matrix->m[9]  * z + matrix->m[13] * w;
    result.z = matrix->m[2] * x + matrix->m[6] * y + matrix->m[10] * z + matrix->m[14] * w;

    // Return the transformed vector
    return result;
}

Matrix4 createTranslationMatrix(float tx, float ty, float tz) {
    Matrix4 matrix = identityMatrix();
    matrix.m[12] = tx;
    matrix.m[13] = ty;
    matrix.m[14] = tz;
    return matrix;
}

Matrix4 createScaleMatrix(float sx, float sy, float sz) {
    Matrix4 matrix = identityMatrix();
    matrix.m[0] = sx;
    matrix.m[5] = sy;
    matrix.m[10] = sz;
    return matrix;
}

Matrix4 createRotationMatrixX(float angle) {
    float rad = angle * 3.14159265 / 180.0f;
    Matrix4 matrix = identityMatrix();
    matrix.m[5] = cos(rad);
    matrix.m[6] = -sin(rad);
    matrix.m[9] = sin(rad);
    matrix.m[10] = cos(rad);
    return matrix;
}

Matrix4 createRotationMatrixY(float angle) {
    float rad = angle * 3.14159265 / 180.0f;
    Matrix4 matrix = identityMatrix();
    matrix.m[0] = cos(rad);
    matrix.m[2] = sin(rad);
    matrix.m[8] = -sin(rad);
    matrix.m[10] = cos(rad);
    return matrix;
}

Matrix4 createRotationMatrixZ(float angle) {
    float rad = angle * 3.14159265 / 180.0f;
    Matrix4 matrix = identityMatrix();
    matrix.m[0] = cos(rad);
    matrix.m[1] = -sin(rad);
    matrix.m[4] = sin(rad);
    matrix.m[5] = cos(rad);
    return matrix;
}

Matrix4 multiplyMatrices(const Matrix4* a, const Matrix4* b) {
    Matrix4 result = identityMatrix();
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            result.m[row + col * 4] = 
                a->m[row + 0] * b->m[col * 4 + 0] +
                a->m[row + 4] * b->m[col * 4 + 1] +
                a->m[row + 8] * b->m[col * 4 + 2] +
                a->m[row + 12] * b->m[col * 4 + 3];
        }
    }
    return result;
}

Vector3 negateVector3(Vector3 v) {
    Vector3 result = { -v.x, -v.y, -v.z };
    return result;
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

int MatchesBodyPartColor(float color[3]) {
    if (color[0] == 1.0f && color[1] == 0.0f && color[2] == 0.0f) {
        return 1; // Head
    } else if (color[0] == 1.0f && color[1] == 1.0f && color[2] == 0.2f) {
        return 2; // Torso
    } else if (color[0] == 0.2f && color[1] == 0.0f && color[2] == 1.0f) {
        return 3; // Left Upper Arm
    } else if (color[0] == 0.0f && color[1] == 0.0f && color[2] == 1.0f) {
        return 4; // Left Forearm
    } else if (color[0] == 0.2f && color[1] == 1.0f && color[2] == 0.0f) {
        return 5; // Right Upper Arm
    } else if (color[0] == 0.0f && color[1] == 1.0f && color[2] == 0.0f) {
        return 6; // Right Forearm
    } else if (color[0] == 1.0f && color[1] == 0.4f && color[2] == 1.0f) {
        return 7; // Left Leg Upper
    } else if (color[0] == 1.0f && color[1] == 0.9f && color[2] == 1.0f) {
        return 8; // Left Leg Lower
    } else if (color[0] == 1.0f && color[1] == 0.0f && color[2] == 1.0f) {
        return 9; // Right Leg Upper
    } else if (color[0] == 1.0f && color[1] == 0.6f && color[2] == 1.0f) {
        return 10; // Right Leg Lower
    } else {
        return 0; // No match
    }
}
