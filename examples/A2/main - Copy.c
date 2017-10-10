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

// constants
const float gravity = -9.8; //gravity
const float milli = 1000.0; //second

typedef struct
{
   float x;
   float y;
   float z;
} Vec3f; //vector variables

typedef struct
{
   // flags
   bool leftClick; //left mouse down
   bool rightClick; //right mouse down

   float lastX; //last x mouse position
   float lastY; //last y mouse position
   float currX; //current x rotation
   float currY; //current y rotation

   float zoom; //current zoom
   float zoomSmooth; //zoom smoothness

   double fov; //field of view
   double aspectRatio; //
   double nearPlane; //
   double farPlane; //
} Camera; //camera variables

typedef struct
{
   float radius; //sphere radius

   float rotationY; //y axis current rotation
   float rotationYInc; //y axis (de)incremental change
   float rotationZ; //z axis current rotation
   float rotationZInc; //z axis (de)incremental change
   float rotationZMax; //maximum rotation
   float rotationZMin; //minimum rotation

   float speed; //current speed
   float speedInc; //(de)increamental change
   float speedMax; //maximum speed
   float speedMin; //minimun speed

   int nv;

   Vec3f *v1, *v2, *v3, *v4;
   Vec3f initPos, initVel, currPos, currVel; //velocity + position values
} Frog; //frog variables

typedef struct
{
   float radius; //log radius
   int nv;

   Vec3f *v1, *v2, *v3, *v4, *v5, *v6;
   Vec3f initPos, currPos; //position values
} Object; //object variables

typedef struct
{
   // flags
   bool debug; //debug mode
   bool axes; //show axis
   bool wireframe; //show wireframe view
   bool normals; //show normals
   bool go; //start/stop

   float segments; //global segments
   float segmentsMax; //maximum segments
   float segmentsMin; //minimum segments

   float startTime; //start time

   float sizeShapeNormals; //shape normals size

   float sizeTangents; //tangents normals size

   float sizeLevelNormals; //level normals size

   float sizeLevel; //level size
} Global; //global variables

Camera camera = 
{
   false, //leftClick
   false, //rightClick
   
   0,     //lastX
   0,     //lastY
   45.0,  //currX
   0,     //currY
   
   -0.1,  //zoom
   100.0, //zoomSmooth
   
   75.0,  //fov
   1.0,   //aspectRatio
   0.01,  //nearPlane
   100.0, //farPlane
};

Frog frog =
{
   0.01,     //radius

   0.0,      //rotationY
   1.0,      //rotationYInc
   0.785398, //rotationZ (start at 45)
   1.0,      //rotationZInc
   0.0,    //rotationZMax
   90.0,     //rotationZMin

   1.0,      //speed
   0.1,      //speedInc
   100.0,    //speedMax
   0.0,      //speedMin
};

Object objects =
{
   0.01 //logRadius
};

Global g =
{
   false, //debug
   true,  //axes
   true,  //wireframe
   true,  //normals
   false, //go

   8.0,   //segments
   128.0, //segmentsMax
   2.0,   //segmentsMin

   0.0,   //startTime

   0.005, //sizeShapeNormals

   0.01,  //sizeTangents

   0.05,   //sizeLevelNormals
   8.0,   //sizeLevel
};

// converts degrees to radians
float degreesToRadians(float degrees)
{
   float radians = degrees * (M_PI / 180);
   return radians;
}

//GLfloat* addVectors(Vec3f orig, Vec3f add)
//{
//   GLfloat *newVector[];
//
//   newVector = calloc(frog.nv, sizeof(Vec3f));
//
//   newVector->x = orig.x + add.x;
//   newVector->y = orig.y + add.y;
//   newVector->z = orig.z + add.z;
//
//   return newVector;
//}

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

// initialise variables
void initVariables()
{
   frog.currVel.x = frog.initVel.x;
   frog.currVel.y = frog.initVel.y;
   frog.currVel.z = frog.initVel.z;

   togglePolygon();

   g.startTime = 0.0;
}

// projectile motion
void calcPosition(float dt) {
   // position
   frog.currPos.x += frog.currVel.x * dt;
   frog.currPos.y += frog.currVel.y * dt;
   frog.currPos.z += frog.currVel.z * dt;

   // velocity
   frog.currVel.y += gravity * dt;
}

// draws a custom line strip
void drawLine(float startX, float startY, float startZ,
   float endX, float endY, float endZ,
   float r, float g, float b)
{
   glColor3f(r, g, b);
   glBegin(GL_LINE_STRIP);
   glVertex3f(startX, startY, startZ);
   glVertex3f(endX, endY, endZ);
   glEnd();
}

// draws axis
void drawAxis(float length)
{
   drawLine(0, 0, 0, length, 0, 0, 1.0, 0, 0); //x (red)

   drawLine(0, 0, 0, 0, length, 0, 0, 1.0, 0); //y (green)

   drawLine(0, 0, 0, 0, 0, length, 0, 0, 1.0); //z (blue)
}

// draws a custom vector line
void drawVector(float x, float y, float z, float a, float b, float c, float s,
   bool normalize, float red, float green, float blue)
{
   float magnitude;

   if (normalize)
   {
      magnitude = sqrt((a * a) + (b * b) + (c * c));
      a /= magnitude;
      b /= magnitude;
      c /= magnitude;
   }

   // scales size + attaches to position
   a = (a * s) + x;
   b = (b * s) + y;
   c = (c * s) + z;

   drawLine(x, y, z, a, b, c, red, green, blue);
}

// draws a custom plane
void drawPlane(float width, float length, float x, float y, float z,
   float red, float green, float blue)
{
   float startW = -width; //start point
   float endW = width; //end point
   float startL = -length;
   float endL = length;
   float rangeW = endW - startW; //length
   float stepW = rangeW / g.segments; //step size
   float rangeL = endL - startL;
   float stepL = rangeL / g.segments;

   //togglePolygon();

   for (float i = startW; i < endW; i += stepW)
   {
      glColor3f(red, green, blue);
      glBegin(GL_TRIANGLE_STRIP);
      for (float j = startL; j <= endL; j += stepL)
      {
         glVertex3f(i + x, y, j + z);
         glVertex3f((i + stepW) + x, y, j + z);
      }
      glEnd();

      for (float j = startL; j <= endL; j += stepL)
      {
         if (g.normals) //toggle normals
         {
            drawVector(i + x, y, j + z, 0, 1, 0, g.sizeLevelNormals, true, 1, 1, 0);
            drawVector((i + stepW) + x, y, j + z, 0, 1, 0, g.sizeLevelNormals, true, 1, 1, 0);
         }
      }
   }
}

// draws a parabola
void drawParabola()
{
   float x, y, z, a, b, c; // start + end point

   setInitVelocity(); //reset values

   float t, tof = (frog.initVel.y * 2) / -gravity; //time of flight

   float step = tof / g.segments; //step size

   // tangents
   for (int i = 0; i <= g.segments; i++)
   {
      t = i * step;

      x = frog.initVel.x * t;
      y = (frog.initVel.y * t) + (0.5 * gravity * t * t);
      z = frog.initVel.z * t;

      a = frog.initVel.x;
      b = frog.initVel.y + gravity * t;
      c = frog.initVel.z;

      if (g.normals) //toggle tangents (normals toggle used, only instance)
      {
         drawVector(x + frog.initPos.x, y + frog.initPos.y, z + frog.initPos.z,
            a, b, c, g.sizeTangents, true, 0.0, 1.0, 1.0);
      }
   }

   // parabola
   glColor3f(0.0, 0.0, 1.0);
   glBegin(GL_LINE_STRIP);
   for (int i = 0; i <= g.segments; i++)
   {
      t = i * step;

      x = frog.initVel.x * t;
      y = (frog.initVel.y * t) + (0.5 * gravity * t * t);
      z = frog.initVel.z * t;

      glVertex3f(x + frog.initPos.x, y + frog.initPos.y, z + frog.initPos.z);
   }
   glEnd();
}

// draws a veloctiy vector
void drawVelocity()
{
   float a, b, c; //end points
   float velScale = 0.05; //scale velocity size

   a = sin(frog.rotationZ) * sin(frog.rotationY) * (frog.speed * velScale);
   b = cos(frog.rotationZ) * (frog.speed * velScale);
   c = sin(frog.rotationZ) * cos(frog.rotationY) * (frog.speed * velScale);

   drawLine(frog.currPos.x, frog.currPos.y, frog.currPos.z, //starting point
      a + frog.currPos.x, b + frog.currPos.y, c + frog.currPos.z, //end point
      1.0, 0.0, 1.0); //color
}

void initFrog()
{
   float theta, phi;
   float stepPhi = M_PI / g.segments;
   float stepTheta = 2.0 * M_PI / g.segments;
   Vec3f *v1, *v2, *v3, *v4;

   float slices = g.segments;
   float stacks = g.segments;

   // allocate arrays + number of vectors
   frog.nv = ((stacks + 1)*(slices + 1));
   frog.v1 = calloc(frog.nv, sizeof(Vec3f));
   frog.v2 = calloc(frog.nv, sizeof(Vec3f));
   frog.v3 = calloc(frog.nv, sizeof(Vec3f));
   frog.v4 = calloc(frog.nv, sizeof(Vec3f));

   if (!frog.v1 || !frog.v2 || !frog.v3 || !frog.v4)
   {
      printf("no\n");
      exit(1);
   } //error

   v1 = frog.v1;
   v2 = frog.v2;
   v3 = frog.v3;
   v4 = frog.v4;

   for (int j = 0; j < stacks; j++)
   {
      phi = j / stacks * M_PI;
      for (int i = 0; i <= slices; i++)
      {
         theta = i / slices * 2.0 * M_PI;

         v1->x = frog.radius * sinf(phi) * cosf(theta);
         v1->y = frog.radius * sinf(phi) * sinf(theta);
         v1->z = frog.radius * cosf(phi);
         v1++;

         v2->x = frog.radius * sinf(phi + stepPhi) * cosf(theta);
         v2->y = frog.radius * sinf(phi + stepPhi) * sinf(theta);
         v2->z = frog.radius * cosf(phi + stepPhi);
         v2++;
         
         v3->x = frog.radius * sinf(phi) * cosf(theta + stepTheta);
         v3->y = frog.radius * sinf(phi) * sinf(theta + stepTheta);
         v3->z = frog.radius * cosf(phi);
         v3++;
         
         v4->x = frog.radius * sinf(phi + stepPhi) * cosf(theta + stepTheta);
         v4->y = frog.radius * sinf(phi + stepPhi) * sinf(theta + stepTheta);
         v4->z = frog.radius * cosf(phi + stepPhi);
         v4++;
      }
   }

   *frog.v1 = *v1;
   *frog.v2 = *v2;
   *frog.v3 = *v3;
   *frog.v4 = *v4;
}

void drawFrog()
{
   drawAxis(1.0);

   //togglePolygon();

   glColor3f(1, 1, 1);
   glBegin(GL_TRIANGLE_STRIP);
   for (int i = 0; i < frog.nv; i++)
   {
      glVertex3fv((float *)&frog.v1[i]);
      glVertex3fv((float *)&frog.v2[i]);
      glVertex3fv((float *)&frog.v3[i]);
      glVertex3fv((float *)&frog.v4[i]);
   }
   glEnd();

   if (g.normals) //toggle normals
   {
      for (int i = 0; i < frog.nv; i++)
      {
         drawVector
         (
            frog.v1[i].x + frog.currPos.x,
            frog.v1[i].y + frog.currPos.y,
            frog.v1[i].z + frog.currPos.z,
            frog.v1[i].x, frog.v1[i].y, frog.v1[i].z,
            g.sizeShapeNormals, true, 1, 1, 0
         );
      }
   }
}

void initLog(float size)
{
   float theta;
   float stepTheta = 2.0 * M_PI / g.segments;
   Vec3f *v1, *v2, *v3, *v4, *v5, *v6;

   float slices = g.segments;

   // allocate arrays
   objects.nv = (slices + 1);
   objects.v1 = calloc(objects.nv, sizeof(Vec3f));
   objects.v2 = calloc(objects.nv, sizeof(Vec3f));
   objects.v3 = calloc(objects.nv, sizeof(Vec3f));
   objects.v4 = calloc(objects.nv, sizeof(Vec3f));
   objects.v5 = calloc(objects.nv, sizeof(Vec3f));
   objects.v6 = calloc(objects.nv, sizeof(Vec3f));

   if (!objects.v1 || !objects.v2 || !objects.v3
      || !objects.v4 || !objects.v5 || !objects.v6)
   {
      printf("no\n");
      exit(1);
   } //error

   v1 = objects.v1;
   v2 = objects.v2;
   v3 = objects.v3;
   v4 = objects.v4;
   v5 = objects.v5;
   v6 = objects.v6;

   for (int i = 0; i <= slices; i++)
   {
      theta = i / slices * 2.0 * M_PI;

      v1->x = -size;
      v1->y = objects.radius * cosf(theta);
      v1->z = objects.radius * sinf(theta);
      v1++;

      v2->x = -size;
      v2->y = objects.radius * cosf(theta + stepTheta);
      v2->z = objects.radius * sinf(theta + stepTheta);
      v2++;

      v3->x = size;
      v3->y = objects.radius * cosf(theta);
      v3->z = objects.radius * sinf(theta);
      v3++;

      v4->x = size;
      v4->y = objects.radius * cosf(theta + stepTheta);
      v4->z = objects.radius * sinf(theta + stepTheta);
      v4++;

      v5->x = -size;
      v5->y = objects.currPos.y;
      v5->z = objects.currPos.z;
      v5++;

      v6->x = size;
      v6->y = objects.currPos.y;
      v6->z = objects.currPos.z;
      v6++;
   }

   *objects.v1 = *v1;
   *objects.v2 = *v2;
   *objects.v3 = *v3;
   *objects.v4 = *v4;
   *objects.v5 = *v5;
   *objects.v6 = *v6;
}

void drawLog()
{
   drawAxis(1.0);

   //togglePolygon();

   glColor3f(1, 1, 1);
   glBegin(GL_TRIANGLE_STRIP);
   for (int i = 0; i < objects.nv; i++)
   {
      glVertex3fv((float *)&objects.v1[i]);
      glVertex3fv((float *)&objects.v2[i]);
      glVertex3fv((float *)&objects.v3[i]);
      glVertex3fv((float *)&objects.v4[i]);
   }
   glEnd();

   glBegin(GL_TRIANGLE_STRIP);
   for (int i = 0; i < objects.nv; i++)
   {
      glVertex3fv((float *)&objects.v5[i]);
      glVertex3fv((float *)&objects.v1[i]);
      glVertex3fv((float *)&objects.v2[i]);
   }
   glEnd();

   glBegin(GL_TRIANGLE_STRIP);
   for (int i = 0; i < objects.nv; i++)
   {
      glVertex3fv((float *)&objects.v6[i]);
      glVertex3fv((float *)&objects.v3[i]);
      glVertex3fv((float *)&objects.v4[i]);
   }
   glEnd();

   if (g.normals) //toggle normals
   {
      for (int i = 0; i < objects.nv; i++)
      {
         drawVector
         (
            objects.v1[i].x + objects.currPos.x,
            objects.v1[i].y + objects.currPos.y,
            objects.v1[i].z + objects.currPos.z,
            objects.v1[i].x + objects.currPos.x,
            (int)objects.v1[i].y + objects.currPos.y,
            (int)objects.v1[i].z + objects.currPos.z,
            g.sizeShapeNormals, true, 1, 1, 0
         );
         drawVector
         (
            objects.v1[i].x + objects.currPos.x,
            objects.v1[i].y + objects.currPos.y,
            objects.v1[i].z + objects.currPos.z,
            0,
            objects.v1[i].y + objects.currPos.y,
            objects.v1[i].z + objects.currPos.z,
            g.sizeShapeNormals, true, 1, 1, 0
         );
         drawVector
         (
            objects.v3[i].x + objects.currPos.x,
            objects.v3[i].y + objects.currPos.y,
            objects.v3[i].z + objects.currPos.z,
            objects.v3[i].x + objects.currPos.x,
            (int)objects.v3[i].y + objects.currPos.y,
            (int)objects.v3[i].z + objects.currPos.z,
            g.sizeShapeNormals, true, 1, 1, 0
         );
         drawVector
         (
            objects.v3[i].x + objects.currPos.x,
            objects.v3[i].y + objects.currPos.y,
            objects.v3[i].z + objects.currPos.z,
            0,
            objects.v3[i].y + objects.currPos.y,
            objects.v3[i].z + objects.currPos.z,
            g.sizeShapeNormals, true, 1, 1, 0
         );
      }
   }
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
      camera.currY += dX; //rotate up
      camera.currX += dY; //rotate down
   }

   // zoom
   if (camera.rightClick) //right mouse down
   {
      camera.zoom += dY / camera.zoomSmooth; //added smoothness
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
         initLog(0.025);
      }
      break;
   case '-': //decrease tesselation
      if (g.segments > g.segmentsMin)
      {
         g.segments /= 2.0;
         initFrog();
         initLog(0.025);
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
      frog.rotationY += degreesToRadians(frog.rotationYInc);
      break;
   case GLUT_KEY_RIGHT: //decrease rotation
      frog.rotationY -= degreesToRadians(frog.rotationYInc);
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

   setInitVelocity();
   initVariables();
   initFrog();
   initLog(0.025);
}

void display()
{
   glLoadIdentity();
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_NORMALIZE);

   glPushMatrix();
   glTranslatef(0.0, 0.0, camera.zoom);
   glRotatef(camera.currY, 0.0, 1.0, 0.0);
   glRotatef(camera.currX, 1.0, 0.0, 0.0);

   // Drawing world axis
   drawAxis(2.0);

   drawPlane(8, 8, 0, 0, 0, 0, 1, 0); //grass
   drawPlane(8, 3, 0, 0.01, 3.8, 0.3, 0.3, 0.3); //road
   drawPlane(8, 3, 0, 0.01, -3.8, 0, 0, 1); //water
   drawFrog();
   //drawLog();
   drawParabola();
   drawVelocity();

   glPopMatrix();

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