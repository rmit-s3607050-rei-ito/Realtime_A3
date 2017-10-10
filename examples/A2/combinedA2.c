#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#if _WIN32
#   include <Windows.h>
#endif
#if __APPLE__
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#   include <GLUT/glut.h>
#else
#   include <GL/gl.h>
#   include <GL/glu.h>
#   include <GL/glut.h>
#endif

#ifndef M_PI
#    define M_PI 4.0 * atan(1.0)
#endif

// ##### ADDED STUFF #####
// Arrays storing vertex points
#define numObj 10
#define numLanes 4
#define numCarIndicies 36           // 2 sets of Vec3 per face (2 triangles drawn) * 6 faces
#define numCarVertexes 24           // 4 points per face * 6 faces

// ##### ADDED STUFF #####
float carLanePos[numLanes] = { 1.5, 2.9, 4.3, 5.7 };
float logLanePos[numLanes] = { -1.5, -2.9, -4.3, -5.7 };

// constants
const float gravity = -9.8; //gravity
const float milli = 1000.0; //second
const int sVertexArray = 4;
const int cVertexArray = 6;

// ##### ADDED STUFF #####
// Lighting
GLfloat lightDirection[] = { 1.0, 1.0, 1.0, 0.0 };     // Light source direction
GLfloat lightIntensity[] = { 1.0, 1.0, 1.0, 0.0 };     // Light intensity
// Ambient and diffuse material color
GLfloat lightGray[] = { 0.1, 0.1, 0.1, 0.0 };          // Frog material color
GLfloat brown[] = { 0.54, 0.27, 0.07, 0.0 };           // Log material color
GLfloat red[] =   { 1.0, 0.0, 0.0, 0.0 };              // Car material color
GLfloat green[] = { 0.0, 1.0, 0.0, 0.0 };              // Grass material color
GLfloat blue[] =  { 0.0, 0.0, 1.0, 0.0 };              // Water/Parabola material color
GLfloat gray[] =  { 0.3, 0.3, 0.3, 0.0 };              // Road material color
GLfloat yellow[] = { 1.0, 1.0, 0.0, 0.0 };             // Normal material color
GLfloat purple[] = { 1.0, 0.0, 1.0, 0.0 };             // Velocity material color
// Specular material color
GLfloat white[] = { 1.0 , 1.0, 1.0, 0.0 };
// Shininess
const int shine = 25;

// ##### ADDED STUFF #####
typedef struct {
    float x, y;
} Vec2f;

typedef struct
{
   float x, y, z;
} Vec3f; //vector variables

// ##### ADDED STUFF #####
typedef struct {
    Vec3f pos;
    Vec3f normal;
    Vec2f texturePos;
} Vertex;

typedef struct
{
   float r, g, b, a;
} Color3f;

typedef struct
{
   // flags
   bool rotateLeftRight; // Rotate camera (y axis)
   bool rotateUpDown; // Rotate camera (x axis)
   bool rotateBoth; // Rotate camera both (y axis and x axis)

   bool leftClick; //left mouse down
   bool rightClick; //right mouse down

   float lastX; //last x mouse position
   float lastY; //last y mouse position
   float xRot; //current x rotation
   float yRot; //current y rotation
   float rotRadius; //radius which camera rotates around frog

   float zoom;       //current zoom
   float zoomSmooth; //zoom smoothness

   Vec3f initPos;   // Inital camera position
   Vec3f currPos;   // Current camera position

   double fov; //field of view
   double aspectRatio; //
   double nearPlane; //
   double farPlane; //
} Camera; //camera variables

// ##### MODIFIED STUFF #####
typedef struct
{
   float nSize; // frog normals size
   float radius; //sphere radius

   float rotationY; //y axis current rotation
   float rotationYInc; //y axis (de)incremental change
   // ##### ADDED STUFF #####
   float rotationYMin; // minimum y rotation
   float rotationYMax; // maximum y rotation

   float rotationZ; //z axis current rotation
   float rotationZInc; //z axis (de)incremental change
   float rotationZMax; //maximum rotation
   float rotationZMin; //minimum rotation

   float speed; //current speed
   float speedInc; //(de)increamental change
   float speedMax; //maximum speed
   float speedMin; //minimun speed

   int nv;

   Vec3f *v[4];
   Vec3f initPos, initVel, currPos, currVel; //velocity + position values
} Frog; //frog variables

typedef struct
{
   float size;
   float nSize;

   float minSpeed;
   float maxSpeed;
} Car;

typedef struct
{
   float radius;
   float size;
   float nSize;

   float minSpeed;
   float maxSpeed;

   int nv;
   Vec3f *v[6];
} Log;

typedef struct
{
   float speed;
   float nv;
   Vec3f *v[6];
   Vec3f initPos, currPos; //position values
} Object; //object variables

// ##### MODIFIED STUFF #####
typedef struct
{
   // flags
   bool debug; //debug mode
   bool axes; //show axis
   bool wireframe; //show wireframe view
   bool normals; //show normals
   bool go; //start/stop
   // ##### ADDED STUFF #####
   bool lighting; // toggle lighting

   float segments; //global segments
   float segmentsMax; //maximum segments
   float segmentsMin; //minimum segments

   float startTime; //start time

   float sizeTangents; //tangents normals size

   float sizeLevelNormals; //level normals size

   float sizeLevel; //level size

   Color3f nColor;
} Global; //global variables

// ##### ADDED STUFF #####
typedef struct {
    Vec3f grassPos, roadPos, waterPos;
    Color3f grassCol, roadCol, waterCol;

    // By default a 8x8 plane, with 8x8 squares
    float size;
    float start;
    float end;
    float range;
} Plane;

Camera camera = 
{
   false, //rotateLeftRight
   false, //rotateUpDown
   true,  //rotateBoth

   false, //leftClick
   false, //rightClick
   

   0,     //lastX
   0,     //lastY
   0,     //xRot
   0,     //yRot
   1.0,   //rotRadius

   0.0,   //zoom
   100.0, //zoomSmooth

   { 0.0, 0.5, 0.0 }, // initPos
   { 0.0, 0.5, 0.0 }, // currPos
   
   75.0,  //fov
   1.0,   //aspectRatio
   0.01,  //nearPlane
   100.0, //farPlane
};

// ##### MODIFIED ######
Frog frog =
{
   0.005,    //nSize
   0.01,     //radius

   // ##### ADDED STUFF ######
   -3.14159, //rotationY (-180 degrees)
    1.0,     //rotationYInc
   -100.0,   //rotationYMin
   -260.0,   //rotationYMax

   0.785398, //rotationZ (start at 45)
   1.0,      //rotationZInc
   0.0,      //rotationZMax
   90.0,     //rotationZMin

   1.0,      //speed
   0.1,      //speedInc
   100.0,    //speedMax
   0.0,      //speedMin
};

Global g =
{
   false, //debug
   true,  //axes
   false,  //wireframe
   true,  //normals
   false, //go
   true,  //lighting

   8.0,   //segments
   128.0, //segmentsMax
   2.0,   //segmentsMin

   0.0,   //startTime

   0.01,  //sizeTangents

   0.25,  //sizeLevelNormals

   8.0,   //sizeLevel

   { 1, 1, 0 },
};

// ##### ADDED STUFF #####
Plane plane = {
    { 0, 0, 0 },             // grassPos
    { 0, 0.01, 3.8 },        // roadPos
    { 0, 0.01, -3.8 },       // waterPos
    
    { 0, 1, 0 },             // grassCol
    { 0.3, 0.3, 0.3 },       // roadCol
    { 0, 0, 1 },             // waterCol

    8.0,                     // size
    -8.0,                    // start
    8.0,                     // end
    16.0,                    // range (end - start)
};

// ##### ADDED STUFF #####
Log logV = {
    0.2,                    // radius
    0.7,                    // size
    0.05,                   // nSize

    0.025,                  // minSpeed
    0.05,                   // maxSpeed
};

// ##### ADDED STUFF #####
Car carV = {
    0.25,                   // size
    0.25,                   // nSize

    0.05,                   // minSpeed
    0.1,                    // maxSpeed
};

// ##### ADDED STUFF #####
Object cars[numObj];
Object logs[numObj];

// ##### ADDED STUFF #####
GLint carIndicies[numCarIndicies] = {
    0, 2, 3,       0, 1, 2,         // Front
    4, 5, 6,       4, 7, 5,         // Back
    8, 9, 10,      8, 11, 9,        // Right
    12, 13, 14,    12, 15, 13,      // Left
    16, 17, 18,    16, 17, 19,      // Top
    20, 21, 22,    20, 23, 21,      // Bottom
};

// ##### ADDED STUFF #####
Vertex carVerticies[numCarVertexes] = {
    // [FORMAT]: { posX, posY, posZ }, { normX, normY, normZ, }, { textX, textY }
    // Front
    { { 1.0, -1.0, 1.0, },  { 0.0, 0.0, 1.0, },  { 0.0, 0.0, }, },  // p0 [0]
    { { 1.0, 1.0, 1.0,  },  { 0.0, 0.0, 1.0, },  { 0.0, 0.0, }, },  // p1 [1]
    { { -1.0, 1.0, 1.0, },  { 0.0, 0.0, 1.0, },  { 0.0, 0.0, }, },  // p2 [2]
    { { -1.0, -1.0, 1.0,},  { 0.0, 0.0, 1.0, },  { 0.0, 0.0, }, },  // p3 [3]
    // Back
    { { 1.0, -1.0, -1.0,},  { 0.0, 0.0, -1.0, }, { 0.0, 0.0, }, },  // p4 [4]
    { { -1.0, 1.0, -1.0,},  { 0.0, 0.0, -1.0, }, { 0.0, 0.0, }, },  // p5 [5]
    { { -1.0, -1.0, -1.0,}, { 0.0, 0.0, -1.0, }, { 0.0, 0.0, }, },  // p6 [6]
    { { 1.0, 1.0, -1.0, },  { 0.0, 0.0, -1.0, }, { 0.0, 0.0, }, },  // p7 [7]
    // Right
    { { 1.0, -1.0, -1.0,},  { 1.0, 0.0, 0.0, },  { 0.0, 0.0, }, },  // p4 [8]
    { { 1.0, 1.0, 1.0,  },  { 1.0, 0.0, 0.0, },  { 0.0, 0.0, }, },  // p1 [9]
    { { 1.0, -1.0, 1.0, },  { 1.0, 0.0, 0.0, },  { 0.0, 0.0, }, },  // p0 [10]
    { { 1.0, 1.0, -1.0, },  { 1.0, 0.0, 0.0, },  { 0.0, 0.0, }, },  // p7 [11]
    // Left
    { { -1.0, -1.0, -1.0,}, { -1.0, 0.0, 0.0, }, { 0.0, 0.0, }, },  // p6 [12]
    { { -1.0, 1.0, 1.0, },  { -1.0, 0.0, 0.0, }, { 0.0, 0.0, }, },  // p2 [13]
    { { -1.0, -1.0, 1.0, }, { -1.0, 0.0, 0.0, }, { 0.0, 0.0, }, },  // p3 [14]
    { { -1.0, 1.0, -1.0, }, { -1.0, 0.0, 0.0, }, { 0.0, 0.0, }, },  // p5 [15]
    // Top
    { { 1.0, 1.0, 1.0,  },  { 0.0, 1.0, 0.0, },  { 0.0, 0.0, }, },  // p1 [16]
    { { -1.0, 1.0, -1.0,},  { 0.0, 1.0, 0.0, },  { 0.0, 0.0, }, },  // p5 [17]
    { { -1.0, 1.0, 1.0, },  { 0.0, 1.0, 0.0, },  { 0.0, 0.0, }, },  // p2 [18]
    { { 1.0, 1.0, -1.0, },  { 0.0, 1.0, 0.0, },  { 0.0, 0.0, }, },  // p7 [19]
    // Bottom
    { { 1.0, -1.0, 1.0, },  { 0.0, -1.0, 0.0, }, { 0.0, 0.0, }, },  // p0 [20]
    { { -1.0, -1.0, -1.0,}, { 0.0, -1.0, 0.0, }, { 0.0, 0.0, }, },  // p6 [21]
    { { -1.0, -1.0, 1.0, }, { 0.0, -1.0, 0.0, }, { 0.0, 0.0, }, },  // p3 [22]
    { { 1.0, -1.0, -1.0, }, { 0.0, -1.0, 0.0, }, { 0.0, 0.0, }, },  // p4 [23]
};

// converts degrees to radians
float degreesToRadians(float degrees)
{
   float radians = degrees * (M_PI / 180);
   return radians;
}

float randomNum(float minNum, float maxNum)
{
   float random = ((float)rand() / RAND_MAX);
   float range = maxNum - minNum;
   float result = (random * range) + minNum;

   return result;
}

Vec3f addVectors(Vec3f orig, Vec3f add)
{
   Vec3f newVector;

   newVector.x = orig.x + add.x;
   newVector.y = orig.y + add.y;
   newVector.z = orig.z + add.z;

   return newVector;
}

// initialise initVel
void setInitVelocity()
{
   frog.initVel.x = sin(frog.rotationZ) * sin(frog.rotationY) * frog.speed;
   frog.initVel.y = cos(frog.rotationZ) * frog.speed;
   frog.initVel.z = sin(frog.rotationZ) * cos(frog.rotationY) * frog.speed;
}

// polygon toggle
void togglePolygon()
{
   if (g.wireframe)
   {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   }
   else
   {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   }
}

// ##### ADDED STUFF #####
void toggleLighting(bool on) 
{
    if (g.lighting) 
    {
        if (on) 
        {
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
        }
        else 
        {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
        }
    }
}

 // ##### ADDED STUFF #####
void setMaterial(GLfloat color[]) {
    // Set material of shapes based on passed color, and white specular with shine
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
}

// ##### MODIFIED FUNCTION #####
// initialise variables
void initVariables()
{
   // ##### ADDED STUFF #####
   // Initialize frog starting position
   frog.initPos.x = 0.0;		frog.initPos.y = 0.0;      frog.initPos.z = 7.5;
   frog.currPos = frog.initPos;

   // Initialize frog velocity
   frog.currVel = frog.initVel;

   // Set drawing mode and time to 0.0 (used for frog animation)
   togglePolygon();
   g.startTime = 0.0;

   // ##### ADDED STUFF #####
   // initialize light
   glLightfv(GL_LIGHT0, GL_POSITION, lightDirection);
   //glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
   //glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
}

// projectile motion
void calcPosition(float dt) {
   // position
   frog.currPos.x += frog.currVel.x * dt;
   frog.currPos.y += frog.currVel.y * dt + 0.5 * gravity * dt * dt;
   frog.currPos.z += frog.currVel.z * dt;

   // velocity
   frog.currVel.y += gravity * dt;

   // Update camera position
   camera.currPos = addVectors(frog.currPos, camera.currPos);
}

// ##### MODIFIED FUNCTION #####
// draws a custom line strip
void drawLine(Vec3f start, Vec3f end, Color3f color)
{
   // Prevent lighting from affecting line
   toggleLighting(false);

   glColor3fv((float *)&color);
   glBegin(GL_LINE_STRIP);
       glVertex3fv((float *)&start);
       glVertex3fv((float *)&end);
   glEnd();

   // Re-enable lighting if lighting applied
   toggleLighting(true);
}

// ##### MODIFIED FUNCTION #####
// draws axis
void drawAxis(Vec3f pos, float length)
{
    if (g.axes) 
    {
        Vec3f xAxis, yAxis, zAxis;
        Color3f red = { 1,0,0 }, green = { 0,1,0 }, blue = { 0,0,1 };

        xAxis = pos, yAxis = pos, zAxis = pos;
        xAxis.x += length;
        yAxis.y += length;
        zAxis.z += length;

        drawLine(pos, xAxis, red);   // x (red)
        drawLine(pos, yAxis, green); // y (green)
        drawLine(pos, zAxis, blue);  // z (blue)
    }
}

// draws a custom vector line
void drawVector(Vec3f start, Vec3f end, float scale, bool normalize, Color3f color)
{
   float magnitude;

   if (normalize)
   {
      magnitude = sqrt((end.x * end.x) + (end.y * end.y) + (end.z * end.z));
      end.x /= magnitude;
      end.y /= magnitude;
      end.z /= magnitude;
   }

   // scales size + attaches to position
   end.x = (end.x * scale) + start.x;
   end.y = (end.y * scale) + start.y;
   end.z = (end.z * scale) + start.z;

   drawLine(start, end, color);
}

// ##### MODIFIED FUNCTION #####
// draws a custom plane
void drawPlane(float width, float length, Vec3f position, Color3f color, GLfloat matColor[])
{
   float startW = -width; //start point
   float endW = width; //end point
   float startL = -length;
   float endL = length;
   float rangeW = endW - startW; //length
   float stepW = rangeW / g.segments; //step size
   float rangeL = endL - startL;
   float stepL = rangeL / g.segments;


   for (float i = startW; i < endW; i += stepW)
   {
      // Add material to plane and set default color
      setMaterial(matColor);
      glColor3fv((float *)&color);

      glBegin(GL_TRIANGLE_STRIP);
      for (float j = startL; j <= endL; j += stepL)
      {
         glVertex3f(i + position.x, position.y, j + position.z);
         glVertex3f((i + stepW) + position.x, position.y, j + position.z);
      }
      glEnd();

      if (g.normals) //toggle normals
      {
         for (float j = startL; j <= endL; j += stepL)
         {
            Vec3f f1 = { i + position.x, position.y, j + position.z };
            Vec3f f2 = { (i + stepW) + position.x, position.y, j + position.z };
            Vec3f nEnd = { 0, 1, 0 };

            drawVector(f1, nEnd, g.sizeLevelNormals, true, g.nColor);
            drawVector(f2, nEnd, g.sizeLevelNormals, true, g.nColor);
         }
      }
   }
}

// draws a parabola
void drawParabola()
{
   Vec3f start, end; // start + end points
   Color3f tColor = { 0, 1, 1 };

   setInitVelocity(); //reset values

   float t, tof = (frog.initVel.y * 2) / -gravity; //time of flight

   float step = tof / g.segments; //step size

   // tangents
   for (int i = 0; i <= g.segments; i++)
   {
      t = i * step;

      start.x = frog.initVel.x * t;
      start.y = (frog.initVel.y * t) + (0.5 * gravity * t * t);
      start.z = frog.initVel.z * t;

      end.x = frog.initVel.x;
      end.y = frog.initVel.y + gravity * t;
      end.z = frog.initVel.z;

      if (g.normals) //toggle tangents (normals toggle used, only instance)
      {
         drawVector(addVectors(start, frog.initPos), end, g.sizeTangents, true, tColor);
      }
   }

   // Set color for lighting
   setMaterial(blue);

   // parabola
   glColor3f(0.0, 0.0, 1.0);
   glBegin(GL_LINE_STRIP);
   for (int i = 0; i <= g.segments; i++)
   {
      t = i * step;

      start.x = frog.initVel.x * t;
      start.y = (frog.initVel.y * t) + (0.5 * gravity * t * t);
      start.z = frog.initVel.z * t;
      start = addVectors(start, frog.initPos);

      glVertex3fv((float *)&start);
   }
   glEnd();
}

// draws a veloctiy vector
void drawVelocity()
{
   Vec3f end; //end points
   Color3f color = { 1, 0, 1 };
   float velScale = 0.05; //scale velocity size

   end.x = sin(frog.rotationZ) * sin(frog.rotationY) * (frog.speed * velScale);
   end.y = cos(frog.rotationZ) * (frog.speed * velScale);
   end.z = sin(frog.rotationZ) * cos(frog.rotationY) * (frog.speed * velScale);
   end = addVectors(end, frog.currPos);

   // Set color for lighting
   setMaterial(purple);

   drawLine(frog.currPos, end, color);
}

void initFrog()
{
   float theta, phi;
   float stepPhi = M_PI / g.segments;
   float stepTheta = 2.0 * M_PI / g.segments;
   Vec3f *v[4];

   float slices = g.segments;
   float stacks = g.segments;

   // allocate arrays + number of vectors
   frog.nv = ((stacks + 1)*(slices + 1));

   for (int i = 0; i < sVertexArray; i++)
   {
      if (frog.v[i] != NULL)
      {
         free(frog.v[i]);
      }

      frog.v[i] = calloc(frog.nv, sizeof(Vec3f));
      if (!frog.v[i])
      {
         printf("no\n");
         exit(1);
      } //error

      v[i] = frog.v[i];
   }

   for (int j = 0; j < stacks; j++)
   {
      phi = j / stacks * M_PI;
      for (int i = 0; i <= slices; i++)
      {
         theta = i / slices * 2.0 * M_PI;

         v[0]->x = frog.radius * sinf(phi) * cosf(theta);
         v[0]->y = frog.radius * sinf(phi) * sinf(theta);
         v[0]->z = frog.radius * cosf(phi);
         v[0]++;

         v[1]->x = frog.radius * sinf(phi + stepPhi) * cosf(theta);
         v[1]->y = frog.radius * sinf(phi + stepPhi) * sinf(theta);
         v[1]->z = frog.radius * cosf(phi + stepPhi);
         v[1]++;
         
         v[2]->x = frog.radius * sinf(phi) * cosf(theta + stepTheta);
         v[2]->y = frog.radius * sinf(phi) * sinf(theta + stepTheta);
         v[2]->z = frog.radius * cosf(phi);
         v[2]++;
         
         v[3]->x = frog.radius * sinf(phi + stepPhi) * cosf(theta + stepTheta);
         v[3]->y = frog.radius * sinf(phi + stepPhi) * sinf(theta + stepTheta);
         v[3]->z = frog.radius * cosf(phi + stepPhi);
         v[3]++;
      }
   }
}

void drawFrog()
{
   Vec3f start;
   Vec3f v[4];

   drawAxis(frog.currPos, 0.05);

   glColor3f(1, 1, 1);
   glBegin(GL_TRIANGLE_STRIP);
   for (int i = 0; i < frog.nv; i++)
   {
      // Set material for frog
      setMaterial(lightGray);
      for (int j = 0; j < sVertexArray; j++)
      {
         v[j] = addVectors(frog.v[j][i], frog.currPos);

         glVertex3fv((float *)&v[j]);
      }
   }
   glEnd();

   if (g.normals) //toggle normals
   {
      // Set material to normals
      setMaterial(yellow);
      for (int i = 0; i < frog.nv; i++)
      {
         for (int j = 0; j < sVertexArray; j++)
         {
            v[j] = addVectors(frog.v[j][i], frog.currPos);
            drawVector(v[j], frog.v[j][i], frog.nSize, true, g.nColor);
         }
      }
   }

   // ##### ADDED STUFF #####
   drawParabola();
   drawVelocity();
}

// ##### ADDED STUFF #####
void initLogPosAndSpeed() {
    // ##### Create and store logs in array #####
    float randXPos;
    float randSpeed;
    Object logObj;
    logObj.initPos.y = 0.0;

    for (int i = 0; i < numObj; i++) {
        randXPos = randomNum(plane.start, plane.end);
        randSpeed = randomNum(logV.minSpeed, logV.maxSpeed);

        logObj.initPos.x = randXPos;                   // Randomize xPosition
        logObj.initPos.z = logLanePos[i % numLanes];   // Add logs lane by lane
        logObj.currPos = logObj.initPos;               // Set current position
        logObj.speed = randSpeed;                      // Set speed

        logs[i] = logObj;

        if (g.debug) {
            printf("Created car at pos: (%f, %f, %f)\n", logObj.initPos.x, logObj.initPos.y, logObj.initPos.z);
            printf("Generated speed: %f\n", randSpeed);
        }
    }

    /* The range which to generate and place logs is from: 6.4 to 0.8
        - There are 4 lanes so the lanes are (spacing is 1.4):
            [LANE1] = 6.4 to 5.0       Midpoint = 5.7
            [LANE2] = 5.0 to 3.6       Midpoint = 4.3
            [LANE3] = 3.6 to 2.2       Midpoint = 2.9
            [LANE4] = 2.2 to 0.8       Midpoint = 1.5
        - Midpoint is where the logs are spawned
    */
}

void initLog()
{
   float theta;
   float stepTheta = 2.0 * M_PI / g.segments;
   Vec3f *v[6];

   float slices = g.segments;

   // allocate arrays
   logV.nv = (slices + 1);

   for (int i = 0; i < cVertexArray; i++)
   {
      if (logV.v[i] != NULL)
      {
         free(logV.v[i]);
      }
      logV.v[i] = calloc(logV.nv, sizeof(Vec3f));
      if (!logV.v[i])
      {
         printf("no\n");
         exit(1);
      } //error

      v[i] = logV.v[i];
   }

   for (int i = 0; i <= slices; i++)
   {
      theta = i / slices * 2.0 * M_PI;

      v[0]->x = -logV.size;
      v[0]->y = logV.radius * cosf(theta);
      v[0]->z = logV.radius * sinf(theta);
      v[0]++;

      v[1]->x = -logV.size;
      v[1]->y = logV.radius * cosf(theta + stepTheta);
      v[1]->z = logV.radius * sinf(theta + stepTheta);
      v[1]++;

      v[2]->x = logV.size;
      v[2]->y = logV.radius * cosf(theta);
      v[2]->z = logV.radius * sinf(theta);
      v[2]++;

      v[3]->x = logV.size;
      v[3]->y = logV.radius * cosf(theta + stepTheta);
      v[3]->z = logV.radius * sinf(theta + stepTheta);
      v[3]++;

      v[4]->x = -logV.size;
      v[4]->y = 0;
      v[4]->z = 0;
      v[4]++;

      v[5]->x = logV.size;
      v[5]->y = 0;
      v[5]->z = 0;
      v[5]++;
   }
}

// ##### MODIFIED #####
void drawLog(Object logObj, Log logV)
{
   Vec3f start, end;
   Vec3f v[6];

   drawAxis(logObj.currPos, 1.0);

   // Add material to car
   setMaterial(brown);

   glColor3f(0.54, 0.27, 0.07);
   glBegin(GL_TRIANGLE_STRIP);
   for (int i = 0; i < logV.nv; i++)
   {
      for (int j = 0; j < cVertexArray - 2; j++)
      {
         v[j] = addVectors(logV.v[j][i], logObj.currPos);
         glVertex3fv((float *)&v[j]);
      }
   }
   glEnd();

   glBegin(GL_TRIANGLE_STRIP);
   for (int i = 0; i < logV.nv; i++)
   {
      v[4] = addVectors(logV.v[4][i], logObj.currPos);
      v[0] = addVectors(logV.v[0][i], logObj.currPos);
      v[1] = addVectors(logV.v[1][i], logObj.currPos);

      glVertex3fv((float *)&v[4]);
      glVertex3fv((float *)&v[0]);
      glVertex3fv((float *)&v[1]);
   }
   glEnd();

   glBegin(GL_TRIANGLE_STRIP);
   for (int i = 0; i < logV.nv; i++)
   {
      v[5] = addVectors(logV.v[5][i], logObj.currPos);
      v[2] = addVectors(logV.v[2][i], logObj.currPos);
      v[3] = addVectors(logV.v[3][i], logObj.currPos);

      glVertex3fv((float *)&v[5]);
      glVertex3fv((float *)&v[2]);
      glVertex3fv((float *)&v[3]);
   }
   glEnd();

   if (g.normals) //toggle normals
   {
      for (int i = 0; i < logV.nv; i++)
      {
         start = addVectors(logV.v[0][i], logObj.currPos);
         end.x = logV.v[0][i].x;
         end.y = (int)logV.v[0][i].y;
         end.z = (int)logV.v[0][i].z;
         drawVector(start, end, logV.nSize, true, g.nColor);

         end.x = 0;
         end.y = logV.v[0][i].y;
         end.z = logV.v[0][i].z;
         drawVector(start, end, logV.nSize, true, g.nColor);

         start = addVectors(logV.v[2][i], logObj.currPos);
         end.x = logV.v[2][i].x;
         end.y = (int)logV.v[2][i].y;
         end.z = (int)logV.v[2][i].z;
         drawVector(start, end, logV.nSize, true, g.nColor);

         end.x = 0;
         end.y = logV.v[0][i].y;
         end.z = logV.v[0][i].z;
         drawVector(start, end, logV.nSize, true, g.nColor);
      }
   }
}

// ##### ADDED STUFF #####
void initCars() {
    // ##### Create and store car positions in array #####
    float randXPos, randSpeed;
    Object car;
    car.initPos.y = 0.0;

    for (int i = 0; i < numObj; i++) {
        // Generate a random x position to start and a random speed value
        randXPos = randomNum(plane.start, plane.end);
        randSpeed = randomNum(carV.minSpeed, carV.maxSpeed);

        car.initPos.x = randXPos;                   // Randomize xPosition
        car.initPos.z = carLanePos[i % numLanes];   // Add cars lane by lane
        car.currPos = car.initPos;                  // Set current position
        car.speed = randSpeed;                      // Set speed

        cars[i] = car;

        if (g.debug) {
            printf("Created car at pos: (%f, %f, %f)\n", car.initPos.x, car.initPos.y, car.initPos.z);
            printf("Generated speed: %f\n", randSpeed);
        }
    }

    /* The range which to generate and place cars is from: 6.4 to 0.8
        - There are 4 lanes so the lanes are (spacing is 1.4):
            [LANE1] = 6.4 to 5.0       Midpoint = 5.7
            [LANE2] = 5.0 to 3.6       Midpoint = 4.3
            [LANE3] = 3.6 to 2.2       Midpoint = 2.9
            [LANE4] = 2.2 to 0.8       Midpoint = 1.5
        - Midpoint is where the cars are spawned
    */
}

// ##### ADDED STUFF #####
void drawCar(Object car) {
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    // Set up pointers
    glNormalPointer(GL_FLOAT, 8 * sizeof(float), carVerticies + 3);   // Add 3 as first set starts at [3]
    glVertexPointer(3, GL_FLOAT, 8 * sizeof(float), carVerticies);

    glPushMatrix();
    // Attach axis and set its color to red
    drawAxis(car.currPos, 0.5);
    glColor3f(1.0, 0.0, 0.0);

    // Move car to position and scale its size
    glTranslatef(car.currPos.x, car.currPos.y, car.currPos.z);
    glScalef(carV.size, carV.size, carV.size);

    // Add material to car
    setMaterial(red);

    // Draw car
    glDrawElements(GL_TRIANGLES, numCarIndicies, GL_UNSIGNED_INT, carIndicies);

    // Attach normals
    if (g.normals) 
    {
        Vec3f point, end;
        for (int i = 0; i < numCarVertexes; i++) 
        {
            point = carVerticies[i].pos;
            end = carVerticies[i].normal;

            drawVector(point, end, carV.nSize, true, g.nColor);
        }
    }

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

// ##### ADDED STUFF #####
void drawLogsAndCars() {
    // Draw randomly generated red cars and brown logs
    for (int i = 0; i < numObj; i++) {
        drawCar(cars[i]);
        drawLog(logs[i], logV);
    }
}

// ##### ADDED STUFF #####
void moveObjects() {
    for (int i = 0; i < numObj; i++) {
        cars[i].currPos.x += cars[i].speed;         // Cars move right -> left
        logs[i].currPos.x += logs[i].speed;         // Logs move left -> right

        // Reset position back to start if out of bounds
        if (cars[i].currPos.x >= plane.size)
            cars[i].currPos.x = plane.start;

        if (logs[i].currPos.x >= plane.size)
            logs[i].currPos.x = plane.start;
    }

    glutPostRedisplay();
}

// ##### ADDED STUFF #####
void setCameraRotation() {
    // Attach frog's current position to camera and add rotation
    // initPos.y is added to ensure camera is mounted slightly above the frog
    camera.currPos.x = frog.currPos.x + 
                       (camera.rotRadius * sin(camera.yRot));

    camera.currPos.y = camera.initPos.y + frog.currPos.y + 
                       (camera.rotRadius * sin(camera.xRot));

    camera.currPos.z = frog.currPos.z + 
                       (camera.rotRadius * cos(camera.xRot) * cos(camera.yRot));
}

// mouse movement events
void mouseMotion(int x, int y)
{
   int dX, dY;

   // calculate last position difference
   dX = x - camera.lastX;
   dY = y - camera.lastY;

   // update previous position
   camera.lastX = x;
   camera.lastY = y;

   // rotation
   if (camera.leftClick) //left mouse down
   {
      // Camera rotation applied to y axis only
      if (camera.rotateLeftRight)
      {
          camera.yRot += dX / camera.zoomSmooth;
      }
      
      // Camera rotation applied to x axis only
      if (camera.rotateUpDown)
      {
         camera.xRot += dY / camera.zoomSmooth;
      }
      
      // Camera rotation applied to both axis
      if (camera.rotateBoth)
      {
          camera.yRot += dX / camera.zoomSmooth;
          camera.xRot += dY / camera.zoomSmooth;
      }

      if (g.debug) {
          printf("Y Rotation: [(L/R) = %f]\n", camera.yRot);
          printf("X Rotation: [(U/D) = %f]\n", camera.xRot);
          printf("Camera position: (%f,%f,%f)\n\n", camera.currPos.x, camera.currPos.y, camera.currPos.z);
      }
   }

   // zoom
   if (camera.rightClick) //right mouse down
   {
      camera.zoom = camera.zoom + dY / camera.zoomSmooth; //added smoothness
   }

   glutPostRedisplay();
}

// mouse click events
void mouseClick(int button, int state, int x, int y)
{
   if (button == 0) //left mouse button
   {
      if (state == 0) //down
      {
         camera.leftClick = true;
         camera.lastX = x; //last mouse position x
         camera.lastY = y; //last mouse position y
      }
      else
      {
         camera.leftClick = false; //reset flag
      }
   }

   if (button == 2) //right mouse button
   {
      if (state == 0) //down
      {
         camera.rightClick = true;
         camera.lastY = y; //last mouse position y
      }
      else
      {
         camera.rightClick = false; //reset flag
      }
   }

   if (button == 3) //mouse scroll up
   {
      if (state == 0) //down
      {
         camera.zoom += 1; //zoom in
      }
   }

   if (button == 4) //mouse scroll down
   {
      if (state == 0) //down
      {
         camera.zoom -= 1; //zoom out
      }
   }

   glutPostRedisplay();
}

// ascii keyboard events
void keyboardASCII(unsigned char key, int x, int y)
{
   switch (key)
   {
   // toggles (on/off)
   case 'o': //axes
      g.axes = !g.axes;
      break;
   case 'n': //normals
      g.normals = !g.normals;
      break;
   case 'p': //wireframe
      g.wireframe = !g.wireframe;
      togglePolygon();
      break;
   case 'l': //lighting
       g.lighting = !g.lighting;
      break;
   case 't': //textures
      break;

   // variable changes
   case 'w': //increase speed
      if (frog.speed <= frog.speedMax)
      {
         frog.speed += frog.speedInc;
      }
      break;
   case 's': //decrease speed
      if (frog.speed >= frog.speedMin)
      {
         frog.speed -= frog.speedInc;
      }
      break;
   case 'a': //increase angle
      if (frog.rotationZ >= degreesToRadians(frog.rotationZMax))
      {
         frog.rotationZ -= degreesToRadians(frog.rotationZInc);
         printf("%f\n", frog.rotationZ);
      }
      break;
   case 'd': //decrease angle
      if (frog.rotationZ <= degreesToRadians(frog.rotationZMin))
      {
         frog.rotationZ += degreesToRadians(frog.rotationZInc);
         printf("%f\n", frog.rotationZ);
      }
      break;
   case '+': //increase tesselation
      if (g.segments < g.segmentsMax)
      {
         g.segments *= 2.0;
         initFrog();
         initLog(0.25);
      }
      break;
   case '-': //decrease tesselation
      if (g.segments > g.segmentsMin)
      {
         g.segments /= 2.0;
         initFrog();
         initLog(0.25);
      }
      break;

   // actions
   case ' ': //jump
      frog.currVel = frog.initVel; //set velocity
      if (!g.go) //start jump
      {
         g.startTime = glutGet(GLUT_ELAPSED_TIME) / (float)milli;
         g.go = true;
      }
      break;

   case 'q': //exit
      exit(EXIT_SUCCESS);
      break;

   // See readme for more information
   case '1':
       camera.rotateLeftRight = true;
       camera.rotateUpDown = false;
       camera.rotateBoth = false;
       break;
   case '2':
       camera.rotateUpDown = true;
       camera.rotateLeftRight = false;
       camera.rotateBoth = false;
       break;
   case '3':
       camera.rotateBoth = true;
       camera.rotateLeftRight = false;
       camera.rotateUpDown = false;
       break;

   default:
      break;
   }

   glutPostRedisplay();
}

// special keyboard events
void keyboardSpecial(int key, int x, int y)
{
   switch (key)
   {
   // variable changes
   case GLUT_KEY_LEFT: //increase rotation
      if (frog.rotationY < degreesToRadians(frog.rotationYMin))
      {
          frog.rotationY += degreesToRadians(frog.rotationYInc);
      }
      break;
   case GLUT_KEY_RIGHT: //decrease rotation
      if (frog.rotationY > degreesToRadians(frog.rotationYMax)) 
      {
          frog.rotationY -= degreesToRadians(frog.rotationYInc);
      }
      break;

   default:
      break;
   }

   glutPostRedisplay();
}

// updating window/display
void update(void)
{
   static float lastT = -1.0;
   float t, dt; //time, delta time

   // ##### ADDED STUFF #####
   moveObjects();    // Move cars/logs

   if (!g.go) //start check
   {
      return;
   }

   t = glutGet(GLUT_ELAPSED_TIME) / milli - g.startTime;

   if (lastT < 0.0)
   {
      lastT = t;
      return;
   }

   dt = t - lastT;

   calcPosition(dt); //projectile motion

   lastT = t;

   // hit ground + reset/set variables
   if (frog.currPos.y < 0.0)
   {
      g.go = false;
      frog.currPos.y = 0.0;                 // Ensure frog and camera doesn't sink
      camera.currPos.y = camera.initPos.y;  // Set camera to default y-pos
         
      frog.initPos = frog.currPos;
      frog.currVel = frog.initVel;
      lastT = -1.0;
   }

   glutPostRedisplay();
}

// scene reshape
void reshape(int width, int height)
{
   if (g.debug)
      printf("[Window dimensions]: [%d,%d]\n", width, height);

   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   camera.aspectRatio = (float)width / (float)height;
   gluPerspective(camera.fov, camera.aspectRatio, camera.nearPlane, camera.farPlane);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

// initialiser 
void init()
{
   glMatrixMode(GL_PROJECTION);
   glMatrixMode(GL_MODELVIEW);

   setInitVelocity();
   initVariables();
   initFrog();

   // ##### ADDED STUFF #####
   // Initialize all background moving shapes
   initLogPosAndSpeed();
   initLog();
   initCars();
}

void display()
{
   Vec3f center = { 0, 0, 0 };

   glLoadIdentity();

   // Enable lighting to materials of shape
   toggleLighting(true);

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_NORMALIZE);

   glPushMatrix();
   glTranslatef(0.0, 0.0, camera.zoom);
   
   // Rotating camera around frog position
   setCameraRotation();

   // Ensure camera is always directed at the frog
   gluLookAt(camera.currPos.x, camera.currPos.y, camera.currPos.z,   // Camera pos
             frog.currPos.x, frog.currPos.y, frog.currPos.z,         // LookAt pos
             0.0, 1.0, 0.0);                                         // Up vector

   // Drawing world axis
   drawAxis(center, 2.0);

   // Drawing individual sections of plane
   drawPlane(plane.size, plane.size, plane.grassPos, plane.grassCol, green); // grass
   drawPlane(plane.size, 3, plane.roadPos, plane.roadCol, gray);             // road
   drawPlane(plane.size, 3, plane.waterPos, plane.waterCol, blue);           // water
   
   // Drawing logs and cars then frog (frog includes parabola and velocity vector)
   drawLogsAndCars();
   drawFrog();

   glPopMatrix();

   // Disable lighting at end of frame
   toggleLighting(false);

   glutSwapBuffers();
}

// main loop
int main(int argc, char **argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize(750, 750);
   glutCreateWindow("Assignment 2");

   init();

   glutMouseFunc(mouseClick);
   glutMotionFunc(mouseMotion);
   glutDisplayFunc(display);
   glutKeyboardFunc(keyboardASCII);
   glutSpecialFunc(keyboardSpecial);
   glutReshapeFunc(reshape);
   glutIdleFunc(update);
   glutMainLoop();

   return EXIT_SUCCESS;
}