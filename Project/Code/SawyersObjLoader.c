//  CSCIx239 library
//  Willem A. (Vlakkies) Schreuder
#include "CSCIx229.h"
#include <ctype.h>

//  Load an OBJ file
//  Vertex, Normal and Texture coordinates are supported
//  Materials are supported
//  Textures must be BMP files
//  Surfaces are not supported
//
//  WARNING:  This is a minimalist implementation of the OBJ file loader.  It
//  will only correctly load a small subset of possible OBJ files.  It is
//  intended to be a starting point to allow you to load models, but in order
//  for you to do this you will have to understand the OBJ format and most
//  likely will have to significantly enhance this code.
//
//  WARNING:  There are lots of really broken OBJ files on the internet.  Some
//  files may have correct surfaces, but the normals are complete junk and so
//  the lighting is totally broken.  So beware of which OBJ files you use.


static float* C;  // Array for vertex colors
static int Nc, Mc;  // Number and max number of colors

//
//  Return true if CR or LF
//
static int CRLF(char ch)
{
   return ch == '\r' || ch == '\n';
}

//
//  Read line from file
//    Returns pointer to line or NULL on EOF
//
static int linelen=0;    //  Length of line
static char* line=NULL;  //  Internal storage for line
static char* readline(FILE* f)
{
   char ch;  //  Character read
   int k=0;  //  Character count
   while ((ch = fgetc(f)) != EOF)
   {
      //  Allocate more memory for long strings
      if (k>=linelen)
      {
         linelen += 8192;
         line = (char*)realloc(line,linelen);
         if (!line) Fatal("Out of memory in readline\n");
      }
      //  End of Line
      if (CRLF(ch))
      {
         // Eat extra CR or LF characters (if any)
         while ((ch = fgetc(f)) != EOF)
           if (!CRLF(ch)) break;
         //  Stick back the overrun
         if (ch != EOF) ungetc(ch,f);
         //  Bail
         break;
      }
      //  Pad character to line
      else
         line[k++] = ch;
   }
   //  Terminate line if anything was read
   if (k>0) line[k] = 0;
   //  Return pointer to line or NULL on EOF
   return k>0 ? line : NULL;
}

//
//  Read to next non-whitespace word
//  Note that this destroys line in the process
//
static char* getword(char** line)
{
   //  Skip leading whitespace
   while (**line && isspace(**line))
      (*line)++;
   if (!**line) return NULL;
   //  Start of word
   char* word = *line;
   //  Read until next whitespace
   while (**line && !isspace(**line))
      (*line)++;
   //  Mark end of word if not NULL
   if (**line)
   {
      **line = 0;
      (*line)++;
   }
   return word;
}

//
//  Read n floats
//
static void readfloat(char* line,int n,float x[])
{
   for (int i=0;i<n;i++)
   {
      char* str = getword(&line);
      if (!str)  Fatal("Premature EOL reading %d floats\n",n);
      if (sscanf(str,"%f",x+i)!=1) Fatal("Error reading float %d\n",i);
   }
}

//
//  Read coordinates
//    n is how many coordiantes to read
//    N is the coordinate index
//    M is the number of coordinates
//    x is the array
//    This function adds more memory as needed in 8192 work chunks
//
static void readcoord(char* line,int n,float* x[],int* N,int* M)
{
   //  Allocate memory if necessary
   if (*N+n > *M)
   {
      *M += 8192;
      *x = (float*)realloc(*x,(*M)*sizeof(float));
      if (!*x) Fatal("Cannot allocate memory\n");
   }
   //  Read n coordinates
   readfloat(line,n,(*x)+*N);
   (*N)+=n;
}

static void readcoordwithcolor(char* line, int n, int c, float* x[], int* N, int* M, float* color[], int* Nc, int* Mc) {
    // Allocate memory if necessary
    if (*N + n > *M) {
        *M += 8192;
        *x = (float*)realloc(*x, (*M) * sizeof(float));
        if (!*x) Fatal("Cannot allocate memory for vertex coordinates\n");
    }
    if (*Nc + c > *Mc) {
        *Mc += 8192;
        *color = (float*)realloc(*color, (*Mc) * sizeof(float));
        if (!*color) Fatal("Cannot allocate memory for vertex colors\n");
    }
    // Read coordinates
    readfloat(line, n, (*x) + *N);
    (*N) += n;
    // Read colors (if provided)
    char* remaining = line + n * 2;  // Assuming each float has a max of 2 characters for simplicity
    for (int i = 0; i < c; i++) {
        char* str = getword(&remaining);
        if (str) {
            if (sscanf(str, "%f", (*color) + (*Nc) + i) != 1) Fatal("Error reading vertex color %d\n", i);
        } else {
            (*color)[(*Nc) + i] = 1.0f;  // Default color to white if not provided
        }
    }
    (*Nc) += c;
}

int SawyersLoadOBJ(const char* file) {
    int Nv, Nn, Nt;  // Number of vertex, normal, and textures
    int Mv, Mn, Mt;  // Maximum vertex, normal, and textures
    Nc = Mc = 0;     // Initialize color counts
    float* V = NULL; // Array of vertexes
    float* N = NULL; // Array of normals
    float* T = NULL; // Array of texture coordinates
    C = NULL;        // Array for vertex colors
    char* line;      // Line pointer
    char* str;       // String pointer

    // Open file
    FILE* f = fopen(file, "r");
    if (!f) Fatal("Cannot open file %s\n", file);

    // Start new display list
    int list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);

    // Read vertexes and facets
    Nv = Nn = Nt = 0;
    Mv = Mn = Mt = 0;
    while ((line = readline(f))) {
        // Vertex coordinates (with colors, if available)
        if (line[0] == 'v' && line[1] == ' ')
            readcoordwithcolor(line + 2, 3, 3, &V, &Nv, &Mv, &C, &Nc, &Mc);
        // Normal coordinates (always 3)
        else if (line[0] == 'v' && line[1] == 'n')
            readcoord(line + 2, 3, &N, &Nn, &Mn);
        // Texture coordinates (always 2)
        else if (line[0] == 'v' && line[1] == 't')
            readcoord(line + 2, 2, &T, &Nt, &Mt);
        // Read and draw facets
        else if (line[0] == 'f') {
            line++;
            glBegin(GL_POLYGON);
            while ((str = getword(&line))) {
                int Kv, Kt, Kn;
                if (sscanf(str, "%d/%d/%d", &Kv, &Kt, &Kn) == 3) {
                    if (Kv < 0 || Kv > Nv / 3) Fatal("Vertex %d out of range 1-%d\n", Kv, Nv / 3);
                    if (Kn < 0 || Kn > Nn / 3) Fatal("Normal %d out of range 1-%d\n", Kn, Nn / 3);
                    if (Kt < 0 || Kt > Nt / 2) Fatal("Texture %d out of range 1-%d\n", Kt, Nt / 2);
                } else if (sscanf(str, "%d//%d", &Kv, &Kn) == 2) {
                    if (Kv < 0 || Kv > Nv / 3) Fatal("Vertex %d out of range 1-%d\n", Kv, Nv / 3);
                    if (Kn < 0 || Kn > Nn / 3) Fatal("Normal %d out of range 1-%d\n", Kn, Nn / 3);
                    Kt = 0;
                } else if (sscanf(str, "%d", &Kv) == 1) {
                    if (Kv < 0 || Kv > Nv / 3) Fatal("Vertex %d out of range 1-%d\n", Kv, Nv / 3);
                    Kn = 0;
                    Kt = 0;
                } else
                    Fatal("Invalid facet %s\n", str);

                // Draw vectors
               float r = C[3 * (Kv - 1) + 0];  // Red component
               float g = C[3 * (Kv - 1) + 1];  // Green component
               float b = C[3 * (Kv - 1) + 2];  // Blue component
               printf("Color of the current vertex is:%f,%f,%f \n", r,g,b); 

               glColor3f(r, g, b);             // Set vertex color
                if (Kt) glTexCoord2fv(T + 2 * (Kt - 1));
                if (Kn) glNormal3fv(N + 3 * (Kn - 1));
                if (Kv) glVertex3fv(V + 3 * (Kv - 1));
            }
            glEnd();
        }
    }
    fclose(f);
    glPopAttrib();
    glEndList();

    // Free arrays
    free(V);
    free(T);
    free(N);
    free(C);

    return list;
}
