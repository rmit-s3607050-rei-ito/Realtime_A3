#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

// Lighting variables
GLfloat lightPosition[] = { 1, 1, 1, 0 };
GLfloat gray[] = { 0.1, 0.1, 0.1, 0 };
GLfloat green[] = { 0.0, 1.0, 0.0, 0.0, 0 };
GLfloat white[] = { 1.0, 1.0, 1.0, 0 };
int shine = 25;

// Frog animation variables
const float g = -9.8;       // Predefined in assignment 1 specs
const float milli = 1000.0;

typedef struct {
	float x;
	float y;
} Vec2f;

typedef struct {
    bool debug;
    bool axes;
    bool wireframe;
    bool normals;

    int maxSegments;
    int minSegments;

    float size;
} Global;

typedef struct {
    // By default a 8x8 grid, with 8x8 squares
    float size;
    float segments;

    float start;
    float end;
    float range;
} Grid;

typedef struct {
    // Drawing frog variables
    int segments;
    float radius;
    float step_theta;
    float step_phi;

    // Animating frog variables
    float rotation;
    float speed;

    float tangentSize;

    Vec2f initPos;
    Vec2f initVel;
    Vec2f currPos;
    Vec2f currVel;
} Frog;

typedef struct {
    float currRotationX;
    float currRotationY;
    int lastX;
    int lastY;

    bool leftClick;        // rotation: left->right, up->down
    bool rightClick;       // zooming:  up->down
} Mouse;

typedef struct {
    float cameraZoom;
    float zoomFactor;

    double fov;
    double aspectRatio;
    double nearPlane;
    double farPlane;
} Camera;

Global global = {
    false,                  // debug
    true,                   // axes
    true,                   // wireframe
    true,                   // normals

    32,                     // maxSegments
    2,                      // minSegments

    0.01
};

Grid grid = {
    8.0,                    // size
    8.0,                    // segments

    0.0,                    // start
    0.0,                    // end
    0.0,                    // range (end - start)
};

Mouse mouse = {
    45.0,                   // currRotationX
    0.0,                    // currRotationY
    0,                      // lastX
    0,                      // lastY

    false,                  // rotate
    false,                  // zoom
};

Camera camera = {
    -0.1,                  // cameraZoom
    100.0,                  // zoomFactor

    75.0,                   // fov
    1.0,                    // aspectRatio
    0.01,                   // nearPlane
    100.0,                  // farPlane
};

Frog frog = {
    8,                      // segments (In this case stacks and slices are equal)
    0.01,                   // radius
    0,                      // step_theta
    0,                      // step_phi

    0.785398,		        // rotation (45 degrees)
    1.0,			        // speed

    0.01,                   // tangentSize

    0,                      // initPos
    0,                      // initVel
    0,                      // currPos
    0,                      // currVel
};

// Set initial parameters
void setInitFrogState() {
    frog.initPos.x = 0.0;		    frog.initPos.y = 0.0;
    frog.currPos.x = 0.0;			frog.currPos.y = 0.0;

    frog.initVel.x = cos(frog.rotation) * frog.speed;
    frog.initVel.y = sin(frog.rotation) * frog.speed;
    frog.currVel.x = cos(frog.rotation) * frog.speed;
    frog.currVel.y = sin(frog.rotation) * frog.speed;
}

void calcInitParams() {
    // ##### Grid variables #####
    grid.start = -grid.size;
    grid.end = grid.size;
    grid.range = grid.end - grid.start;

    // ##### Frog variables #####
    frog.step_theta = 2.0 * M_PI / frog.segments;
    frog.step_phi = M_PI / frog.segments;

    if (global.debug)
        printf(" - step_theta: %f\n - step_phi: %f\n", frog.step_theta, frog.step_phi);
}

// Add/Reduce grid number of columns and rows
void changeTesselation(bool increase) {
    // (Min = 2, Max = 32), since all variables have an initial value of 8, only 1 check is needed
    //if (increase) {
    //    if (grid.nextSquare < global.maxSegments) {

    //    }
    //    
    //}
    //else {
    //    if (grid.nextSquare > global.minSegments) {
    //        grid.nextSquare /= 2;
    //        frog.segments /= 2;
    //    }
    //}

    // Recalculate rest of variables given changes
    calcInitParams();
}

// ##### Drawing functions #####
void drawLineStrip(float startX, float startY, float startZ, // Start coordinates 
    float endX, float endY, float endZ,                      // End coordinates
    float red, float green, float blue)                      // Color
{   // [Usage:] For drawing the velocity vector and axis
    glColor3f(red, green, blue);

    glBegin(GL_LINE_STRIP);
        glVertex3f(startX, startY, startZ);
        glVertex3f(endX, endY, endZ);
    glEnd();

    glColor3f(1.0, 1.0, 1.0); // Switch color back for other drawing functions
}

// takes in required variables and draws a line strip
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

// draw a vecotr line with given variables
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
   a = (a * (s * global.size)) + x;
   b = (b * (s * global.size)) + y;
   c = (c * (s * global.size)) + z;

   drawLine(x, y, z, a, b, c, red, green, blue);
}

void drawVectorE(float startX, float startY, float startZ, // Start coordinates
    float endX, float endY, float endZ,       // End coordinates
    float sizeScale, bool normalize,          // Whether scaled
    float red, float green, float blue)       // Color
{   // [Usage:] For drawing parabola tangents
    float x = endX;
    float y = endY;

    if (normalize) {
        float magnitude;
        magnitude = sqrt((endX * endX) + (endY * endY));
        x /= magnitude;
        y /= magnitude;
    }

    // Scale vector size
    x *= sizeScale;         y *= sizeScale;

    // Shift position to next strip
    x += startX;            y += startY;

    drawLineStrip(startX, startY, startZ,
                  x, y, endZ,
                  red, green, blue);
}

void drawAxis(float length) {
    if (global.axes) {
        // x-axis (red)
        drawLineStrip(0.0, 0.0, 0.0,
            length, 0.0, 0.0,
            1.0, 0.0, 0.0);

        // y-axis (green)
        drawLineStrip(0.0, 0.0, 0.0,
            0.0, length, 0.0,
            0.0, 1.0, 0.0);

        // z-axis (blue)
        drawLineStrip(0.0, 0.0, 0.0,
            0.0, 0.0, length,
            0.0, 0.0, 1.0);
    }
}

void drawNormal(float size, float x, float y, float z) {
    // Ensure lighting doesnt apply to normals
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);

    // Yellow
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINES);
        glVertex3f(x, y, z);
        glVertex3f(x, y + size, z);
    glEnd();
    
    // Set color back to white
    glColor3f(1.0, 1.0, 1.0);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
}

void drawPlane(float size)
{
   float start = -size;
   float end = size;
   float range = end - start;
   float step = range / g.segments;

   if (g.wireframe)
   {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   }
   else
   {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   }

   for (float i = start; i < end; i += step)
   {
      glColor3f(0, 1, 0);
      glBegin(GL_TRIANGLE_STRIP);
      for (float j = start; j <= end; j += step)
      {
         glVertex3f(i, 0.0, j);
         glVertex3f(i + step, 0.0, j);
      }
      glEnd();

      for (float j = start; j <= end; j += step)
      {
         if (g.normals)
         {
            drawVector(i, 0, j, 0, 1, 0, g.sizeLevelNormals, true, 1, 1, 0);
            drawVector(i + step, 0, j, 0, 1, 0, g.sizeLevelNormals, true, 1, 1, 0);
         }
      }
   }
}

void drawParabola() {
    // ##### Note: This is drawn via parametric method #####
    glColor3f(0.0, 0.0, 1.0); // Set color to blue
    
    // Update velocity based on rotation + speed
    frog.initVel.x = cos(frog.rotation) * frog.speed;
    frog.initVel.y = sin(frog.rotation) * frog.speed;

    float tof = (frog.initVel.y * 2) / -g; // tof = time of flight
    float stepSize = tof / frog.segments;
    float t, x, y;

    float endX, endY; // Used for drawing tangents and normals

    glBegin(GL_LINE_STRIP);
        for (float i = 0; i <= frog.segments; i++) {
            t = i * stepSize;

            x = frog.initVel.x * t;                       // x = v*t
            y = (frog.initVel.y * t) + (0.5 * g * t * t); // y = v*t - 1/2*g*t^2

            x += frog.initPos.x;
            y += frog.initPos.y;

            glVertex3f(x, y, 0.0);
        }
    glEnd();

    // Attach tangents
    if (global.normals) {
        for (float i = 0; i <= frog.segments; i++) {
            // original coordinates of parabola
            t = i * stepSize;
            x = frog.initVel.x * t;                       // x = v*t
            y = (frog.initVel.y * t) + (0.5 * g * t * t); // y = v*t - 1/2*g*t^2

            endX = frog.initVel.x;
            endY = frog.initVel.y + (g * t);              // derivative of above y equation

            x += frog.initPos.x;
            y += frog.initPos.y;

            drawVector(x, y, 0.0, endX, endY, 0.0, frog.tangentSize, true, 0.0, 1.0, 1.0);
        }
    }
}

void drawFrog() {
    drawAxis(1.0);

    float slices = (float)frog.segments;
    float stacks = (float)frog.segments;
    float theta, phi;
    float x1, y1, z1,
          x2, y2, z2,
          x3, y3, z3,
          x4, y4, z4;

    if (global.wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (int j = 0; j <= stacks; j++) {
        phi = j / stacks * M_PI;
        for (int i = 0; i <= slices; i++) {
            theta = i / slices * 2.0 * M_PI;
            x1 = frog.radius * sinf(phi) * cosf(theta);
            y1 = frog.radius * sinf(phi) * sinf(theta);
            z1 = frog.radius * cosf(phi);

            x2 = frog.radius * sinf(phi + frog.step_phi) * cosf(theta);
            y2 = frog.radius * sinf(phi + frog.step_phi) * sinf(theta);
            z2 = frog.radius * cosf(phi + frog.step_phi);

            x3 = frog.radius * sinf(phi) * cosf(theta + frog.step_theta);
            y3 = frog.radius * sinf(phi) * sinf(theta + frog.step_theta);
            z3 = frog.radius * cosf(phi);

            x4 = frog.radius * sinf(phi + frog.step_phi) * cosf(theta + frog.step_theta);
            y4 = frog.radius * sinf(phi + frog.step_phi) * sinf(theta + frog.step_theta);
            z4 = frog.radius * cosf(phi + frog.step_phi);

            glColor3f(1, 1, 1);
            glBegin(GL_QUAD_STRIP);
                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);
                glVertex3f(x3, y3, z3);
                glVertex3f(x4, y4, z4);
            glEnd();

            if (global.normals)
            {
                drawVector(x1, y1, z1, x1, y1, z1, 0.5, true, 1, 1, 0);
            }
        }
    }
}

void drawLog(size)
{
   drawAxis(1.0);

   float slices = g.segments;
   float theta;
   float thetaStep = 2.0 * M_PI / g.segments;
   float x1, y1, z1,
      x2, y2, z2,
      x3, y3, z3,
      x4, y4, z4;
   float a1, b1, c1,
      a2, b2, c2;

   if (g.wireframe)
   {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   }
   else
   {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   }

   for (int i = 0; i <= slices; i++)
   {
      theta = i / slices * 2.0 * M_PI;

      x1 = frog.radius * cosf(theta);
      y1 = frog.radius * sinf(theta);
      z1 = -size;

      x2 = frog.radius * cosf(theta + thetaStep);
      y2 = frog.radius * sinf(theta + thetaStep);
      z2 = -size;

      x3 = frog.radius * cosf(theta);
      y3 = frog.radius * sinf(theta);
      z3 = size;

      x4 = frog.radius * cosf(theta + thetaStep);
      y4 = frog.radius * sinf(theta + thetaStep);
      z4 = size;

      a1 = x1;
      b1 = y1;
      c1 = z1;
      a2 = x3;
      b2 = y3;
      c2 = z3;

      x1 += frog.currPos.x, x2 += frog.currPos.x,
         x3 += frog.currPos.x, x4 += frog.currPos.x;
      y1 += frog.currPos.y, y2 += frog.currPos.y,
         y3 += frog.currPos.y, y4 += frog.currPos.y;
      z1 += frog.currPos.z, z2 += frog.currPos.z,
         z3 += frog.currPos.z, z4 += frog.currPos.z;

      glColor3f(1, 1, 1);
      glBegin(GL_TRIANGLE_STRIP);
      glVertex3f(x1, y1, z1);
      glVertex3f(x2, y2, z2);
      glVertex3f(x3, y3, z3);
      glVertex3f(x4, y4, z4);
      glEnd();

      glBegin(GL_TRIANGLE_STRIP);
      glVertex3f(0, 0, z1);
      glVertex3f(x1, y1, z1);
      glVertex3f(x2, y2, z2);
      glEnd();

      glBegin(GL_TRIANGLE_STRIP);
      glVertex3f(0, 0, z3);
      glVertex3f(x3, y3, z3);
      glVertex3f(x4, y4, z4);
      glEnd();


      if (g.normals)
      {
         drawVector(x1, y1, z1, (int)a1, (int)b1, c1, g.sizeShapeNormals, true, 1, 1, 0);
         drawVector(x1, y1, z1, a1, b1, 0, g.sizeShapeNormals, true, 1, 1, 0);
         drawVector(x3, y3, z3, (int)a2, (int)b2, c2, g.sizeShapeNormals, true, 1, 1, 0);
         drawVector(x3, y3, z3, a2, b2, 0, g.sizeShapeNormals, true, 1, 1, 0);
      }
   }
}

// Mouse functions
void mouseMotion(int x, int y) {
    int dX, dY;

    // Calculate difference in mouse movement given its previous position
    dX = x - mouse.lastX;
    dY = y - mouse.lastY;
    // Update last pos after calculating deltaX,Y to be position mouse moved to
    mouse.lastX = x;
    mouse.lastY = y;

    // Rotate only when left mouse button is clicked
    if (mouse.leftClick) {
        // Rotate teapot based on this rotation difference for both left->right, up->down
        mouse.currRotationY += dX;
        mouse.currRotationX += dY;
    }

    // Zoom when right mouse button only, zoom factor to ensure zoom is smooth
    if (mouse.rightClick)
        camera.cameraZoom += dY / camera.zoomFactor;

    glutPostRedisplay();
}

void mouseClick(int button, int state, int x, int y) {
    // ##### Mouse clicks #####
    // # 0 = left click,        1 = middle mouse,       2 = right click

    // ##### States #####
    // # 0 = mouse held down,   1 = mouse released

    // Left click
    if (button == 0) {
        // When mouse clicked, store last position of mouse
        if (state == 0) {
            mouse.leftClick = true;
            mouse.lastX = x;
            mouse.lastY = y;
        } else // Reset flag to prevent rotation after mouse released
            mouse.leftClick = false;
    }

    // Right click
    if (button == 2) {
        if (state == 0) {
            mouse.rightClick = true;
            mouse.lastY = y;
        } else
            mouse.rightClick = false;
    }

    if (button == 3) {
       if (state == 0)
         camera.cameraZoom += 1;
    }

    if (button == 4) {
       if (state == 0)
         camera.cameraZoom -= 1;
    }

    glutPostRedisplay();
}

// Keyboard functions
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'o':
            global.axes = !global.axes;
            break;
        case 'n':
            global.normals = !global.normals;
            break;
        case 'p':
            global.wireframe = !global.wireframe;
            break;
        case '+':
           if (grid.segments < 200)
           {
              grid.segments *= 2;
              frog.segments *= 2;
           }
           calcInitParams();
           break;
        case '-':
           if (grid.segments > 1)
           {
              grid.segments /= 2;
              frog.segments /= 2;
           }
           calcInitParams();
           break;
        case 27:
        case 'q':
            exit(EXIT_SUCCESS);
            break;
        default:
            break;
    }

    glutPostRedisplay();
}

// Reshaping window/ display
void reshape(int width, int height) {
    if (global.debug)
        printf("[Window dimensions]: [%d,%d]\n", width, height);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    camera.aspectRatio = (float)width / (float)height;
    gluPerspective(camera.fov, camera.aspectRatio, camera.nearPlane, camera.farPlane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void init() {
    // Setting global variables that cannot be hardcoded/change with flags
    // Zooming via moving teapot along z axis, default is zooming via scaling
    glMatrixMode(GL_PROJECTION);
    glMatrixMode(GL_MODELVIEW);

    calcInitParams();
    setInitFrogState();
}

void display() {
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glPushMatrix();
        glTranslatef(0.0, 0.0, camera.cameraZoom);
        glRotatef(mouse.currRotationY, 0.0, 1.0, 0.0);
        glRotatef(mouse.currRotationX, 1.0, 0.0, 0.0);

        // Drawing world axis
        drawAxis(2.0);

        drawGrid();
        //drawFrog();
        drawLog();
        //drawParabola();

    glPopMatrix();

    glutSwapBuffers();
}

int main(int argc, char **argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize(750, 750);
   glutCreateWindow("Assignment 2");

   init();

   glutMouseFunc(mouseClick);
   glutMotionFunc(mouseMotion);
   glutDisplayFunc(display);
   glutKeyboardFunc(keyboard);
   glutReshapeFunc(reshape);
   glutMainLoop();

   return EXIT_SUCCESS;
}