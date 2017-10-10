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

#include <GL/SOIL.h>;

#ifndef M_PI
#    define M_PI 4.0 * atan(1.0)
#endif

// defined constants
#define P_ARRAY_SIZE 4
#define S_ARRAY_SIZE 4
#define C_ARRAY_SIZE 6
#define NUM_OBJECTS 10
#define NUM_LANES 4
#define NUM_CAR_INDECIES 36
#define NUM_CAR_VERTEXES 24

// constants
const float gravity = -9.8; //gravity
const float milli = 1000.0; //second

// lighting
GLfloat lightDirection[] = { 1.0, 1.0, 1.0, 0.0 }; //light source direction
GLfloat lightIntensity[] = { 1.0, 1.0, 1.0, 1.0 }; //light intensity

// ambient and diffuse material colors
GLfloat lightGray[] = { 0.1, 0.1, 0.1, 0.0 }; //frog
GLfloat brown[] = { 0.54, 0.27, 0.07, 0.0 }; //log
GLfloat red[] = { 1.0, 0.0, 0.0, 0.0 }; //car
GLfloat green[] = { 0.0, 1.0, 0.0, 0.0 }; //grass
GLfloat blue[] = { 0.0, 0.0, 1.0, 0.0 }; //water + parabola
GLfloat gray[] = { 0.3, 0.3, 0.3, 0.0 }; //road
GLfloat yellow[] = { 1.0, 1.0, 0.0, 0.0 }; //normals
GLfloat purple[] = { 1.0, 0.0, 1.0, 0.0 }; //velocity

// specular material color
GLfloat white[] = { 1.0 , 1.0, 1.0, 0.0 };

// shininess
const int shine = 25;

typedef struct
{
   float x, y;
} Vec2f;

typedef struct
{
   float x, y, z;
} Vec3f; //vector variables

typedef struct
{
   float r, g, b;
} Color3f; //color variables

typedef struct
{
   Vec3f pos;
   Vec3f normal;
   Vec2f texturePos;
} Vertex; //vertex variables

Vertex carVerticies[NUM_CAR_VERTEXES] =
{
   //{ posX, posY, posZ }, { normX, normY, normZ, }, { textX, textY }
   // front
   { { 1.0, -1.0, 1.0, },{ 0.0, 0.0, 1.0, },{ 0.0, 0.0, }, }, //p0 [0]
   { { 1.0, 1.0, 1.0, },{ 0.0, 0.0, 1.0, },{ 0.0, 0.0, }, }, //p1 [1]
   { { -1.0, 1.0, 1.0, },{ 0.0, 0.0, 1.0, },{ 0.0, 0.0, }, }, //p2 [2]
   { { -1.0, -1.0, 1.0, },{ 0.0, 0.0, 1.0, },{ 0.0, 0.0, }, }, //p3 [3]
   // back
   { { 1.0, -1.0, -1.0, },{ 0.0, 0.0, -1.0, },{ 0.0, 0.0, }, }, //p4 [4]
   { { -1.0, 1.0, -1.0, },{ 0.0, 0.0, -1.0, },{ 0.0, 0.0, }, }, //p5 [5]
   { { -1.0, -1.0, -1.0, },{ 0.0, 0.0, -1.0, },{ 0.0, 0.0, }, }, //p6 [6]
   { { 1.0, 1.0, -1.0, },{ 0.0, 0.0, -1.0, },{ 0.0, 0.0, }, }, //p7 [7]
   // right
   { { 1.0, -1.0, -1.0, },{ 1.0, 0.0, 0.0, },{ 0.0, 0.0, }, }, //p4 [8]
   { { 1.0, 1.0, 1.0, },{ 1.0, 0.0, 0.0, },{ 0.0, 0.0, }, }, //p1 [9]
   { { 1.0, -1.0, 1.0, },{ 1.0, 0.0, 0.0, },{ 0.0, 0.0, }, }, //p0 [10]
   { { 1.0, 1.0, -1.0, },{ 1.0, 0.0, 0.0, },{ 0.0, 0.0, }, }, //p7 [11]
   // left
   { { -1.0, -1.0, -1.0, },{ -1.0, 0.0, 0.0, },{ 0.0, 0.0, }, }, //p6 [12]
   { { -1.0, 1.0, 1.0, },{ -1.0, 0.0, 0.0, },{ 0.0, 0.0, }, }, //p2 [13]
   { { -1.0, -1.0, 1.0, },{ -1.0, 0.0, 0.0, },{ 0.0, 0.0, }, }, //p3 [14]
   { { -1.0, 1.0, -1.0, },{ -1.0, 0.0, 0.0, },{ 0.0, 0.0, }, }, //p5 [15]
   // top
   { { 1.0, 1.0, 1.0, },{ 0.0, 1.0, 0.0, },{ 0.0, 0.0, }, }, //p1 [16]
   { { -1.0, 1.0, -1.0, },{ 0.0, 1.0, 0.0, },{ 0.0, 0.0, }, }, //p5 [17]
   { { -1.0, 1.0, 1.0, },{ 0.0, 1.0, 0.0, },{ 0.0, 0.0, }, }, //p2 [18]
   { { 1.0, 1.0, -1.0, },{ 0.0, 1.0, 0.0, },{ 0.0, 0.0, }, }, //p7 [19]
   // bottom
   { { 1.0, -1.0, 1.0, },{ 0.0, -1.0, 0.0, },{ 0.0, 0.0, }, },  //p0 [20]
   { { -1.0, -1.0, -1.0, },{ 0.0, -1.0, 0.0, },{ 0.0, 0.0, }, }, //p6 [21]
   { { -1.0, -1.0, 1.0, },{ 0.0, -1.0, 0.0, },{ 0.0, 0.0, }, }, //p3 [22]
   { { 1.0, -1.0, -1.0, },{ 0.0, -1.0, 0.0, },{ 0.0, 0.0, }, }, //p4 [23]
}; //assign car variables

typedef struct
{
   // flags
   bool rotateLeftRight; //rotate camera (y axis)
   bool rotateUpDown; //rotate camera (x axis)
   bool rotateBoth; //rotate camera both (y axis and x axis)
   bool leftClick; //left mouse down
   bool rightClick; //right mouse down

   float lastXP; //last x mouse position
   float lastYP; //last y mouse position
   float currXR; //current x rotation
   float currYR; //current y rotation

   float rotRadius; //radius which camera rotates around frog

   float zoom; //current zoom
   float zoomMax; //maximum zoom
   float zoomMin; //minimum zoom
   float zoomSmooth; //zoom smoothness

   Vec3f initPos;   //inital camera position
   Vec3f currPos;   //current camera position

   double fov; //field of view
   double aspectRatio; //view ratio
   double nearPlane; //cut off close
   double farPlane; //cut off far
} Camera; //camera variables

Camera camera =
{
   false, //rotateLeftRight
   false, //rotateUpDown
   true,  //rotateBoth
   false, //leftClick
   false, //rightClick

   0, //lastX
   0, //lastY
   0, //currX
   0, //currY

   1.0, //rotRadius

   0, //zoom
   1, //zoomMax
   0, //zoomMin
   100.0, //zoomSmooth

   { 0.0, 0.5, 0.0 }, //initPos
   { 0.0, 0.5, 0.0 }, //currPos

   75.0, //fov
   1.0, //aspectRatio
   0.01, //nearPlane
   100.0, //farPlane
}; //assign camera variables

/* attempted plane variables
typedef struct
{
   float normals;

   float nv;
   Vec3f *v[P_ARRAY_SIZE];
} Plane;

Plane plane =
{
   0.1,
};
*/

typedef struct
{
   Vec3f grassPos, roadPos, waterPos; //various positions
   Color3f grassColor, roadColor, waterColor; //various colors

   float size; //total size
   float start; //start position
   float end; //end position
   float range; //distance between (end - start)

   float normals; //normal size
} Plane; //plane variables

Plane plane =
{
   { 0, 0, 0 }, //grassPos
   { 0, 0.01, 3.8 }, //roadPos
   { 0, 0.01, -3.8 }, //waterPos

   { 0, 1, 0 }, //grassColor
   { 0.3, 0.3, 0.3 }, //roadColor
   { 0, 0, 1 }, //waterColor

   8.0, //size
   -8.0, //start
   8.0, //end
   16.0, //range

   0.1, //normals
}; //assign plane variables

typedef struct
{
   float radius; //sphere radius
   float normals; //noromal size
   float tangents; //tangent size

   float rotationY; //y axis current rotation
   float rotationYInc; //y axis (de)incremental change
   float rotationYMax; // maximum rotation
   float rotationYMin; // minimum rotation

   float rotationZ; //z axis current rotation
   float rotationZInc; //z axis (de)incremental change
   float rotationZMax; //maximum rotation
   float rotationZMin; //minimum rotation

   float speed; //current speed
   float speedInc; //(de)increamental change
   float speedMax; //maximum speed
   float speedMin; //minimun speed

   int nv; //number of vertices

   Vec3f *v[S_ARRAY_SIZE]; //array of vertex pointers (for multiple vertex sets)
   Vec3f initPos, initVel, currPos, currVel; //velocity + position values
} Frog; //frog variables

Frog frog =
{
   0.01, //radius
   0.005, //normals
   0.01, //tangents

   -3.17159, //rotationY
   1.0, //rotationYInc
   -260.0, //rotationYMax
   -100.0, //rotationYMin

   0.785398, //rotationZ (start at 45)
   1.0, //rotationZInc
   0.0, //rotationZMax
   90.0, //rotationZMin

   1.0, //speed
   0.1, //speedInc
   100.0, //speedMax
   0.0, //speedMin
}; //assign frog variables

typedef struct
{
   float radius; //circle size
   float size; //cylinder length
   float normals; //normals size

   float minSpeed; //lowest speed
   float maxSpeed; //highest speed

   int nv;  //number of vertices
   Vec3f *v[C_ARRAY_SIZE]; //array of vertex pointers (for multiple vertex sets)
} Log; //log variables

Log logV =
{
   0.2, //radius
   0.7, //size
   0.2, //normals

   0.025, //minSpeed
   0.05, //maxSpeed
}; //assign log variables

typedef struct
{
   float size; //cube size
   float normals; //normals size

   float minSpeed; //lowest speed
   float maxSpeed; //highest speed
} Car; //car variables

Car carV =
{
   0.25, //size
   0.25, //normals

   0.05, //minSpeed
   0.1, //maxSpeed
}; //assign car variables

typedef struct
{
   float speed; //objects assigned speed

   Vec3f initPos, currPos; //position values
} Object; //object variables

typedef struct
{
   // flags
   bool debug; //debug mode
   bool axes; //show axis
   bool wireframe; //show wireframe view
   bool normals; //show normals
   bool lighting; //toggle lighting
   bool textures; //toggle textures
   bool go; //start/stop

   float segments; //global segments
   float segmentsMax; //maximum segments
   float segmentsMin; //minimum segments

   float startTime; //start time

   float sizeLevel; //level size

   float logLanePos[NUM_LANES]; //log z positions
   float carLanePos[NUM_LANES]; //car z positions

   Color3f nColor; //normals color

   Object logs[NUM_OBJECTS]; //amount of logs
   Object cars[NUM_OBJECTS]; //amount of cars
} Global; //global variables

Global g =
{
   false, //debug
   true, //axes
   true, //wireframe
   true, //normals
   true, // lighting
   true, // textures
   false, //go

   8.0, //segments
   128.0, //segmentsMax
   2.0, //segmentsMin

   0.0, //startTime

   8.0, //sizeLevel

   { -1.5, -2.9, -4.3, -5.7 }, //logLanePos
   { 1.5, 2.9, 4.3, 5.7 }, //carLanePos

   { 1, 1, 0 }, //nColor
}; //assign global variables

GLint carIndicies[NUM_CAR_INDECIES] =
{
   0, 2, 3,       0, 1, 2, //front
   4, 5, 6,       4, 7, 5, //back
   8, 9, 10,      8, 11, 9, //right
   12, 13, 14,    12, 15, 13, //left
   16, 17, 18,    16, 17, 19, //top
   20, 21, 22,    20, 23, 21, //bottom
}; //assign indicies

// converts degrees to radians
float degreesToRadians(float degrees)
{
   float radians = degrees * (M_PI / 180);
   return radians;
}

// generates a random number between a given min and max
float randomNum(float minNum, float maxNum)
{
   float random = ((float)rand() / RAND_MAX);
   float range = maxNum - minNum;
   float result = (random * range) + minNum;

   return result;
}

// adds passed in vectors together
Vec3f addVectors(Vec3f orig, Vec3f add)
{
   Vec3f newVector;

   newVector.x = orig.x + add.x;
   newVector.y = orig.y + add.y;
   newVector.z = orig.z + add.z;

   return newVector;
}

// normalizes a given vector
Vec3f normalizeVector(Vec3f vector)
{
   float magnitude;

   magnitude = sqrt((vector.x * vector.x)
      + (vector.y * vector.y)
      + (vector.z * vector.z));
   vector.x /= magnitude;
   vector.y /= magnitude;
   vector.z /= magnitude;

   return vector;
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

// lighting toggle
void toggleLighting(bool on)
{
   if (g.lighting)
   {
      if (on) //pass in function toggle (for lines)
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

// set given material
void setMaterial(GLfloat color[])
{
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
}

// initialise variables
void initVariables()
{
   // frog position
   frog.initPos.z = 7.5;
   frog.currPos.z = 7.5;

   // current velocity
   frog.currVel.x = frog.initVel.x;
   frog.currVel.y = frog.initVel.y;
   frog.currVel.z = frog.initVel.z;

   // polygon mode
   togglePolygon();

   // time
   g.startTime = 0.0;

   // light lighting
   glLightfv(GL_LIGHT0, GL_POSITION, lightDirection);
   //glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
   //glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
}

// projectile motion
void calcPosition(float dt)
{
   // position
   frog.currPos.x += frog.currVel.x * dt;
   frog.currPos.y += frog.currVel.y * dt + 0.5 * gravity * dt * dt;
   frog.currPos.z += frog.currVel.z * dt;

   // velocity
   frog.currVel.y += gravity * dt;

   // camera position
   camera.currPos = addVectors(camera.currPos, frog.currPos);
}

// draws a custom line strip
void drawLine(Vec3f start, Vec3f end, Color3f color)
{
   toggleLighting(false); //prevent lighting from affecting line

   glColor3fv((float *)&color); //set color
   glBegin(GL_LINE_STRIP);
   glVertex3fv((float *)&start);
   glVertex3fv((float *)&end);
   glEnd();

   toggleLighting(true); //re-enable lighting
}

// draws axis
void drawAxis(Vec3f pos, float length)
{
   if (g.axes)
   {
      Vec3f axis; //x, y, z have unique values, can be one vector
      Color3f red = { 1,0,0 }, green = { 0,1,0 }, blue = { 0,0,1 }; //colors

      axis = pos; //assign to parent position

      axis.x += length; //x
      axis.y += length; //y
      axis.z += length; //z

      drawLine(pos, axis, red); //x (red)
      drawLine(pos, axis, green); //y (green)
      drawLine(pos, axis, blue); //z (blue)
   }
}

// draws a custom vector line
void drawVector(Vec3f start, Vec3f end, float scale, Color3f color)
{
   end = normalizeVector(end); //normalize end point

   // scales size + attaches to position
   end.x = (end.x * scale) + start.x;
   end.y = (end.y * scale) + start.y;
   end.z = (end.z * scale) + start.z;

   drawLine(start, end, color);
}

/* attempted to store the plane into an array and redraw along with tessalation
   but was ultimatly unsuccessful. But some feedback of what I did wrong would be helpful.

void initPlane(float width, float length, Vec3f position)
{
   float rows = width;
   float columns = length;

   float stepWidth = rows / g.segments;
   float stepLength = columns / g.segments;

   //float stepWidth = width / g.segments;
   //float stepLength = length / g.segments;

   Vec3f *v[P_ARRAY_SIZE];

   plane.nv = (rows + 1)*(columns + 1) - 56;

   for (int i = 0; i < P_ARRAY_SIZE; i++)
   {
      if (plane.v[i] != NULL)
      {
         free(plane.v[i]);
      }

      plane.v[i] = calloc(plane.nv, sizeof(Vec3f));
      if (!plane.v[i])
      {
         printf("no\n");
         exit(1);
      } //error

      v[i] = plane.v[i];
   }

   float count = 0;
   for (float i = 0; i <= rows; i += stepWidth)
   {
      for (float j = 0; j <= columns; j += stepLength)
      {
         count++;
         printf("point: %f\n", count);
         //printf("i: %f\n", i);
         //printf("j: %f\n", j);
         printf("stepWidth: %f\n", stepWidth);
         //printf("stepLength: %f\n", stepLength);
         v[0]->x = i;
         v[0]->y = 0;
         v[0]->z = j;
         v[0]++;
      }
   }
}

// draws a custom plane
void drawPlane(Color3f color)
{
   Vec3f end = { 0, 1, 0 };

   glColor3fv((float *)&color);
   glBegin(GL_POINTS);
   for (int i = 0; i < plane.nv; i++)
   {
      for (int j = 0; j < P_ARRAY_SIZE; j++)
      {
         glVertex3fv((float *)&plane.v[j][i]);
      }
   }
   glEnd();

   if (g.normals) //toggle normals
   {
      for (int i = 0; i < plane.nv; i++)
      {
         for (int j = 0; j < P_ARRAY_SIZE; j++)
         {
            drawVector(plane.v[j][i], end, plane.normals, true, g.nColor);
         }
      }
   }
}
*/

// draws a custom plane
void drawPlane(float width, float length, Vec3f position,
   Color3f color, GLfloat matColor[])
{
   float startW = -width; //width start point
   float endW = width; //width end point
   float startL = -length; //length start point
   float endL = length; //length end poitn
   float stepW = (endW - startW) / g.segments; //width step size
   float stepL = (endL - startL) / g.segments; //length step size

   Vec3f vertices, normal; //stores current vectors

   for (float i = startW; i <= endW; i += stepW) //width loop (columns)
   {
      setMaterial(matColor);//add material
      glColor3fv((float *)&color); //set default color
      glBegin(GL_TRIANGLE_STRIP);
      for (float j = startL; j <= endL; j += stepL) //length loop (rows)
      {
         // first set
         Vec3f v1 = { i, 0, j };
         vertices = addVectors(v1, position); //move to position
         normal = normalizeVector(v1); //set normals
         glNormal3fv((float *)&normal); //draw normals
         glVertex3fv((float *)&vertices); //draw vectors

         // second set
         Vec3f v2 = { (i + stepW), 0, j };
         normal = normalizeVector(v2);
         vertices = addVectors(v2, position);
         glNormal3fv((float *)&normal);
         glVertex3fv((float *)&vertices);
      }
      glEnd();

      if (g.normals) //toggle normals
      {
         for (float j = startL; j <= endL; j += stepL) // normals length loop
         {
            Vec3f f1 = { i + position.x, position.y, j + position.z };
            Vec3f f2 = { (i + stepW) + position.x, position.y, j + position.z };
            Vec3f nEnd = { 0, 1, 0 };

            drawVector(f1, nEnd, plane.normals, g.nColor);
            drawVector(f2, nEnd, plane.normals, g.nColor);
         }
      }
   }
}

// draws a parabola
void drawParabola()
{
   Vec3f start, end; // start + end points
   Color3f tColor = { 0, 1, 1 }; // tangent color

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

      if (g.normals) //toggle tangents (normals toggle used since only tangent instance)
      {
         drawVector(addVectors(start, frog.initPos), end, frog.tangents, tColor);
      }
   }

   // parabola
   toggleLighting(false); //disable lighting
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
   toggleLighting(true); //re-enable lighting
}

// draws a veloctiy vector
void drawVelocity()
{
   Vec3f end; //end points
   Color3f color = { 1, 0, 1 };
   float velScale = 0.05; //scale velocity size

   // assign velocity equasion
   end.x = sin(frog.rotationZ) * sin(frog.rotationY) * (frog.speed * velScale);
   end.y = cos(frog.rotationZ) * (frog.speed * velScale);
   end.z = sin(frog.rotationZ) * cos(frog.rotationY) * (frog.speed * velScale);
   end = addVectors(end, frog.currPos);

   drawLine(frog.currPos, end, color);
}

// create + store sphere points in array
void initFrog()
{
   float theta, phi;
   float stepPhi = M_PI / g.segments;
   float stepTheta = 2.0 * M_PI / g.segments;
   Vec3f *v[S_ARRAY_SIZE]; //array of vertex pointters

   float slices = g.segments;
   float stacks = g.segments;

   // number of sphere vertices
   frog.nv = ((stacks + 1)*(slices + 1));

   // allocate array memory
   for (int i = 0; i < S_ARRAY_SIZE; i++)
   {
      if (frog.v[i] != NULL) //free used memory
      {
         free(frog.v[i]);
      }

      frog.v[i] = calloc(frog.nv, sizeof(Vec3f));

      if (!frog.v[i])
      {
         printf("no\n");
         exit(1);
      } //error

      v[i] = frog.v[i]; //assign to frog vectors
   }

   for (int j = 0; j < stacks; j++)
   {
      phi = j / stacks * M_PI;
      for (int i = 0; i <= slices; i++)
      {
         theta = i / slices * 2.0 * M_PI;

         // assigning multiple vertices
         //sphere points
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

// draws array vertices
void drawFrog()
{
   Vec3f start;
   Vec3f v[S_ARRAY_SIZE]; //store vertices
   Vec3f n; //store normals

   drawAxis(frog.currPos, 0.05);

   setMaterial(lightGray); //set material
   glColor3f(1, 1, 1);
   glBegin(GL_TRIANGLE_STRIP);
   for (int i = 0; i < frog.nv; i++) //loop through number of vertices
   {
      for (int j = 0; j < S_ARRAY_SIZE; j++) //loop through vector array
      {
         // assign the vertices to be connected
         v[j] = addVectors(frog.v[j][i], frog.currPos); //add current position
         n = normalizeVector(frog.v[j][i]); //normalizer
         glNormal3fv((float *)&n);
         glVertex3fv((float *)&v[j]);
      }
   }
   glEnd();

   if (g.normals) //toggle normals
   {
      for (int i = 0; i < frog.nv; i++) //loop through number of vertices
      {
         for (int j = 0; j < S_ARRAY_SIZE; j++) //loop through vector array
         {
            v[j] = addVectors(frog.v[j][i], frog.currPos); //store stating point
            drawVector(v[j], frog.v[j][i], frog.normals, g.nColor); //draw normals
         }
      }
   }

   drawParabola(); //draw jump parabola
   drawVelocity(); //draw velocity vector
}

// create + store logs in array
void initLogPosAndSpeed()
{
   float randXPos;
   float randSpeed;
   Object logObj;
   logObj.initPos.y = 0.0;

   for (int i = 0; i < NUM_OBJECTS; i++)
   {
      randXPos = randomNum(plane.start, plane.end);
      randSpeed = randomNum(logV.minSpeed, logV.maxSpeed);

      logObj.initPos.x = randXPos; //randomize xPosition
      logObj.initPos.z = g.logLanePos[i % NUM_LANES]; //add logs lane by lane
      logObj.currPos = logObj.initPos; //set current position
      logObj.speed = randSpeed; //set speed

      g.logs[i] = logObj; //put log into stage array

      if (g.debug)
      {
         printf("Created car at pos: (%f, %f, %f)\n",
            logObj.initPos.x,
            logObj.initPos.y,
            logObj.initPos.z);
         printf("Generated speed: %f\n", randSpeed);
      }
   }
}

// create + store sphere points in array
void initLog(float size)
{
   float theta;
   float stepTheta = 2.0 * M_PI / g.segments;
   Vec3f *v[C_ARRAY_SIZE]; //vector array storage

   float slices = g.segments;

   // number of log vertices
   logV.nv = (slices + 1);

   // allocate array memory
   for (int i = 0; i < C_ARRAY_SIZE; i++)
   {
      if (logV.v[i] != NULL) //free used memory
      {
         free(logV.v[i]);
      }

      logV.v[i] = calloc(logV.nv, sizeof(Vec3f));

      if (!logV.v[i])
      {
         printf("no\n");
         exit(1);
      } //error

      v[i] = logV.v[i]; //assign to log vectors
   }

   for (int i = 0; i <= slices; i++)
   {
      theta = i / slices * 2.0 * M_PI;

      // assigning multiple vertices to vector arrays
      // left circle points
      v[0]->x = -size;
      v[0]->y = logV.radius * cosf(theta);
      v[0]->z = logV.radius * sinf(theta);
      v[0]++;

      // left circle faces
      v[1]->x = -size;
      v[1]->y = logV.radius * cosf(theta + stepTheta);
      v[1]->z = logV.radius * sinf(theta + stepTheta);
      v[1]++;

      // right circle points
      v[2]->x = size;
      v[2]->y = logV.radius * cosf(theta);
      v[2]->z = logV.radius * sinf(theta);
      v[2]++;

      // right circle faces
      v[3]->x = size;
      v[3]->y = logV.radius * cosf(theta + stepTheta);
      v[3]->z = logV.radius * sinf(theta + stepTheta);
      v[3]++;

      // left cicle centre
      v[4]->x = -size;
      v[4]->y = 0;
      v[4]->z = 0;
      v[4]++;

      // right circle centre
      v[5]->x = size;
      v[5]->y = 0;
      v[5]->z = 0;
      v[5]++;
   }
}

void drawLog(Log log, Object obj)
{
   Vec3f start, end;
   Vec3f v[C_ARRAY_SIZE];
   Vec3f n;

   drawAxis(obj.currPos, 1.0);

   setMaterial(brown); //set material

   glColor3f(0.54, 0.27, 0.07); //set color

   // cylinder shaft
   glBegin(GL_TRIANGLE_STRIP);
   for (int i = 0; i < log.nv; i++) //loop through number of vertices
   {
      for (int j = 0; j < C_ARRAY_SIZE - 2; j++) //remove last two points
      {
         // assign the vertices to be drawn
         v[j] = addVectors(log.v[j][i], obj.currPos);
         n = normalizeVector(log.v[j][i]);
         glNormal3fv((float *)&n);
         glVertex3fv((float *)&v[j]);
      }
   }
   glEnd();

   // left face
   glBegin(GL_TRIANGLE_STRIP);
   for (int i = 0; i < log.nv; i++) //loop through number of vertices
   {
      // assign the vertices to be drawn
      v[4] = addVectors(log.v[4][i], obj.currPos);
      v[0] = addVectors(log.v[0][i], obj.currPos);
      v[1] = addVectors(log.v[1][i], obj.currPos);

      n = normalizeVector(log.v[4][i]);
      glNormal3fv((float *)&n);
      glVertex3fv((float *)&v[4]);

      n = normalizeVector(log.v[0][i]);
      glNormal3fv((float *)&n);
      glVertex3fv((float *)&v[0]);

      n = normalizeVector(log.v[1][i]);
      glNormal3fv((float *)&n);
      glVertex3fv((float *)&v[1]);
   }
   glEnd();

   // right face
   glBegin(GL_TRIANGLE_STRIP);
   for (int i = 0; i < log.nv; i++) //loop through number of vertices
   {
      // assign the vertices to be drawn
      v[5] = addVectors(log.v[5][i], obj.currPos);
      v[2] = addVectors(log.v[2][i], obj.currPos);
      v[3] = addVectors(log.v[3][i], obj.currPos);

      n = normalizeVector(log.v[5][i]);
      glNormal3fv((float *)&n);
      glVertex3fv((float *)&v[5]);

      n = normalizeVector(log.v[2][i]);
      glNormal3fv((float *)&n);
      glVertex3fv((float *)&v[2]);

      n = normalizeVector(log.v[3][i]);
      glNormal3fv((float *)&n);
      glVertex3fv((float *)&v[3]);
   }
   glEnd();

   if (g.normals) //toggle normals
   {
      for (int i = 0; i < log.nv; i++) //loop through number of vertices
      {
         start = addVectors(log.v[0][i], obj.currPos); //set start to the left
         //shaft left
         end.x = log.v[0][i].x;
         end.y = (int)log.v[0][i].y; //cast to int to stay straight
         end.z = (int)log.v[0][i].z;
         drawVector(start, end, logV.normals, g.nColor);
         // face left
         end.x = 0;
         end.y = log.v[0][i].y;
         end.z = log.v[0][i].z;
         drawVector(start, end, logV.normals, g.nColor);

         start = addVectors(log.v[2][i], obj.currPos); //set start to the left
         // shaft left
         end.x = log.v[2][i].x;
         end.y = (int)log.v[2][i].y;
         end.z = (int)log.v[2][i].z;
         drawVector(start, end, logV.normals, g.nColor);
         // face right
         end.x = 0;
         end.y = log.v[2][i].y;
         end.z = log.v[2][i].z;
         drawVector(start, end, logV.normals, g.nColor);
      }
   }
}

//  create and store car positions in array
void initCars()
{
   float randXPos;
   float randSpeed;
   Object car;
   car.initPos.y = 0.0;

   for (int i = 0; i < NUM_OBJECTS; i++)
   {
      randXPos = randomNum(plane.start, plane.end);
      randSpeed = randomNum(carV.minSpeed, carV.maxSpeed);

      car.initPos.x = randXPos; //randomize xPosition
      car.initPos.z = g.carLanePos[i % NUM_LANES]; //add cars lane by lane
      car.currPos = car.initPos; //set current position
      car.speed = randSpeed; //set speed

      g.cars[i] = car;

      if (g.debug)
      {
         printf("Created car at pos: (%f, %f, %f)\n",
            car.initPos.x,
            car.initPos.y,
            car.initPos.z);
         printf("Generated speed: %f\n", randSpeed);
      }
   }
}

// draw car
void drawCar(Object car)
{
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_VERTEX_ARRAY);

   // set up pointers
   glNormalPointer(GL_FLOAT, 8 * sizeof(float), carVerticies + 3); //add 3 as first set starts at [3]
   glVertexPointer(3, GL_FLOAT, 8 * sizeof(float), carVerticies);

   glPushMatrix();

   drawAxis(car.currPos, 0.5); //attatch axis
   glColor3f(1.0, 0.0, 0.0); //set color red

   glTranslatef(car.currPos.x, car.currPos.y, car.currPos.z); //move to position
   glScalef(carV.size, carV.size, carV.size); //scale size

   setMaterial(red);

   glDrawElements(GL_TRIANGLES, NUM_CAR_INDECIES, GL_UNSIGNED_INT, carIndicies); // draw car

   if (g.normals) //toggle normals
   {
      Vec3f point, end;
      for (int i = 0; i < NUM_CAR_VERTEXES; i++)
      {
         point = carVerticies[i].pos;
         end = carVerticies[i].normal;

         drawVector(point, end, carV.normals, g.nColor);
      }
   }

   glPopMatrix();

   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
}

// draw randomly generated red cars and brown logs
void drawLogsAndCars()
{
   for (int i = 0; i < NUM_OBJECTS; i++)
   {
      drawCar(g.cars[i]);
      drawLog(logV, g.logs[i]);
   }
}

// object movment
void moveObjects()
{
   for (int i = 0; i < NUM_OBJECTS; i++)
   {
      g.cars[i].currPos.x += g.cars[i].speed; //cars move right -> left
      g.logs[i].currPos.x += g.logs[i].speed; //logs move left -> right

      // reset position back to start if out of bounds
      if (g.cars[i].currPos.x >= plane.size)
      {
         g.cars[i].currPos.x = plane.start;
      }
      if (g.logs[i].currPos.x >= plane.size)
      {
         g.logs[i].currPos.x = plane.start;
      }
   }

   glutPostRedisplay();
}

// attach frog's current position to camera and add rotation
void setCameraRotation()
{
   // initPos.y is added to ensure camera is mounted slightly above the frog
   camera.currPos.x = frog.currPos.x +
      (camera.rotRadius * sin(camera.currYR));

   camera.currPos.y = camera.initPos.y + frog.currPos.y +
      (camera.rotRadius * sin(camera.currXR));

   camera.currPos.z = frog.currPos.z +
      (camera.rotRadius * cos(camera.currXR) * cos(camera.currYR));
}

// mouse movement events
void mouseMotion(int x, int y)
{
   int dX, dY;

   // calculate last position difference
   dX = x - camera.lastXP;
   dY = y - camera.lastYP;

   // update previous position
   camera.lastXP = x;
   camera.lastYP = y;

   // rotation
   if (camera.leftClick) //left mouse down
   {// Camera rotation applied to y axis only
      if (camera.rotateLeftRight)
      {
         camera.currYR += dX / camera.zoomSmooth;
      }

      // Camera rotation applied to x axis only
      if (camera.rotateUpDown)
      {
         camera.currXR += dY / camera.zoomSmooth;
      }

      // Camera rotation applied to both axis
      if (camera.rotateBoth)
      {
         camera.currYR += dX / camera.zoomSmooth; //rotate up
         camera.currXR += dY / camera.zoomSmooth; //rotate down
      }
   }

   // zoom
   if (camera.rightClick) //right mouse down
   {
      camera.zoom += dY / camera.zoomSmooth;
      if (camera.zoom >= camera.zoomMax || camera.zoom <= camera.zoomMin) //zoom lmits
      {
         camera.zoom -= dY / camera.zoomSmooth; //added smoothness
      }
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
         camera.lastXP = x; //last mouse position x
         camera.lastYP = y; //last mouse position y
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
         camera.lastYP = y; //last mouse position y
      }
      else
      {
         camera.rightClick = false; //reset flag
      }
   }

   // mouse scroll for ease
   if (button == 3) //mouse scroll up
   {
      if (state == 0) //down
      {
         camera.zoom += 0.1; //zoom in
      }
   }

   if (button == 4) //mouse scroll down
   {
      if (state == 0) //down
      {
         camera.zoom -= 0.1; //zoom out
      }
   }

   glutPostRedisplay();
}

// ascii keyboard events
void keyboardASCII(unsigned char key, int x, int y)
{
   Vec3f grassPos = { -4, 0, -4 };

   switch (key)
   {
   // toggles (on/off)
   case 'o': //axes
      g.axes = !g.axes;
      printf("[AXIS] %s\n", g.axes ? "on":"off");
      break;
   case 'n': //normals
      g.normals = !g.normals;
      printf("[NORMALS] %s\n", g.normals ? "on" : "off");
      break;
   case 'p': //wireframe
      g.wireframe = !g.wireframe;
      printf("[WIREFRAME] %s\n", g.wireframe ? "on" : "off");
      togglePolygon();
      break;
   case 'l': //lighting
      g.lighting = !g.lighting;
      printf("[LIGHTING] %s\n", g.lighting ? "on" : "off");
      break;
   case 't': //textures
      g.textures = !g.textures;
      printf("[TEXTURES] %s\n", g.textures ? "on" : "off");
      break;

   // variable changes
   case 'w': //increase speed
      if (frog.speed <= frog.speedMax)
      {
         frog.speed += frog.speedInc;
         printf("[SPEED] %3.2f\n", frog.speed);
      }
      break;
   case 's': //decrease speed
      if (frog.speed >= frog.speedMin)
      {
         frog.speed -= frog.speedInc;
         printf("[SPEED] %3.2f\n", frog.speed);
      }
      break;
   case 'a': //increase angle
      if (frog.rotationZ >= degreesToRadians(frog.rotationZMax))
      {
         frog.rotationZ -= degreesToRadians(frog.rotationZInc);
         printf("[ROTATION Z] %3.2f\n", frog.rotationZ);
      }
      break;
   case 'd': //decrease angle
      if (frog.rotationZ <= degreesToRadians(frog.rotationZMin))
      {
         frog.rotationZ += degreesToRadians(frog.rotationZInc);
         printf("[ROTATION Z] %3.2f\n", frog.rotationZ);
      }
      break;
   case '+': //increase tesselation
      if (g.segments < g.segmentsMax)
      {
         g.segments *= 2.0;
         initFrog();
         initLog(logV.size);
         printf("[SEGEMENTS] %3.2f\n", g.segments);
      }
      break;
   case '-': //decrease tesselation
      if (g.segments > g.segmentsMin)
      {
         g.segments /= 2.0;
         initFrog();
         initLog(logV.size);
         printf("[SEGEMENTS] %3.2f\n", g.segments);
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

   // camera conditions (see readme)
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
         printf("[ROTATION Y] %3.2f\n", g.segments);
      }
      break;
   case GLUT_KEY_RIGHT: //decrease rotation
      if (frog.rotationY > degreesToRadians(frog.rotationYMax))
      {
         frog.rotationY -= degreesToRadians(frog.rotationYInc);
         printf("[ROTATION Y] %3.2f\n", g.segments);
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

   //moveObjects(); //start object animation

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
      frog.currPos.y = 0.0;
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

   // variables
   setInitVelocity();
   initVariables();

   // shapes + objects
   initFrog();
   initLogPosAndSpeed();
   initLog(logV.size);
   initCars();
}

void display()
{
   Vec3f center = { 0, 0, 0 };

   glLoadIdentity();

   toggleLighting(true); //apply lighting to materials of shape

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_NORMALIZE);

   glPushMatrix();

   glTranslatef(0.0, 0.0, camera.zoom);

   // rotating camera around frog position
   setCameraRotation();

   // ensure camera is always directed at the frog
   gluLookAt(camera.currPos.x, camera.currPos.y, camera.currPos.z, //camera pos
      frog.currPos.x, frog.currPos.y, frog.currPos.z, //lookAt pos
      0.0, 1.0, 0.0); //up vector

   // Drawing world axis
   drawAxis(center,2.0);

   drawPlane(plane.size, plane.size, plane.grassPos, plane.grassColor, green); // grass
   drawPlane(plane.size, 3, plane.roadPos, plane.roadColor, gray);             // road
   drawPlane(plane.size, 3, plane.waterPos, plane.waterColor, blue);           // water

   drawLogsAndCars();
   drawFrog();

   glPopMatrix();

   toggleLighting(false); //disable lighting at end of frame

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