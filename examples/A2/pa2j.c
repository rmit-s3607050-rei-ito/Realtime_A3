#include "stdafx.h"
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

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
    float z;
} Vec3f;

typedef struct {
    bool debug;
    bool axes;
    bool wireframe;
    bool normals;
    bool go;

    float segments;
    float maxSegments;
    float minSegments;
} Global;

typedef struct {
    // By default a 8x8 grid, with 8x8 squares
    float size;
    float start;
    float end;
    float range;
} Grid;

typedef struct {
    // Drawing frog variables
    float radius;
    float step_theta;
    float step_phi;

    // Animating frog variables
    float zRotation;
    float minZRotation;
    float maxZRotation;

    float speed;
    float speedChange;
    float minSpeed;

    float yRotation;

    float tangentSize;
    float velocityVectorSize;
} Frog;

typedef struct {
    bool go;

    Vec3f initPos;
    Vec3f initVel;
    Vec3f currPos;
    Vec3f currVel;
    
    float startTime;
} FrogState;

typedef struct {
    int lastX;
    int lastY;

    bool leftClick;        // rotation: left->right, up->down
    bool rightClick;       // zooming:  up->down
} Mouse;

typedef struct {
    float currRotationX;
    float currRotationY;
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
    false,                  // go

    8.0,                    // segments
    64.0,                   // maxSegments
    2.0,                    // minSegments
};

Grid grid = {
    8.0,                    // size
    0.0,                    // start
    0.0,                    // end
    0.0,                    // range (end - start)
};

Mouse mouse = {
    0,                      // lastX
    0,                      // lastY

    false,                  // rotate
    false,                  // zoom
};

Camera camera = {
    45.0,                   // currRotationX
    0.0,                    // currRotationY
    -0.1,                   // cameraZoom
    100.0,                  // zoomFactor

    75.0,                   // fov
    1.0,                    // aspectRatio
    0.01,                   // nearPlane
    100.0,                  // farPlane
};

Frog frog = {
    0.01,                   // radius
    0,                      // step_theta
    0,                      // step_phi

    0.785398,		        // zRotation (45 degrees)
    0.0,                    // minRotation
    180.0,                  // maxRotation
    1.0,			        // speed
    0.1,                    // speedChange
    0.0,                    // minSpeed

    0.0,                    // yRotation

    0.02,                   // tangentSize
    0.05,                   // velocityVectorSize
};

FrogState state;

// ##### Updating/Conversion variable functions #####
void changeTesselation(bool increase) {
    if (increase) {
        if (global.segments < global.maxSegments)
            global.segments *= 2.0;
    }
    else {
        if (global.segments > global.minSegments)
            global.segments /= 2.0;
    }

    // Update frog step_phi and step_theta
    frog.step_theta = 2.0 * M_PI / global.segments;
    frog.step_phi = M_PI / global.segments;

    if (global.debug)
        printf("Segments: %f\n", global.segments);
}

float degreesToRadians(float degrees) {
    float radians = degrees * (M_PI / 180.0);
    return radians;
}

// ##### Setting/Changing initial parameters #####
void setInitFrogState() {
    state.initPos.x = 0.0;		state.initPos.y = 0.0;      state.initPos.z = 0.0;
    state.currPos.x = 0.0;		state.currPos.y = 0.0;      state.currPos.z = 0.0;

    state.initVel.x = sin(frog.zRotation) * sin(frog.yRotation) * frog.speed;
    state.initVel.y = cos(frog.zRotation) * frog.speed;
    state.initVel.z = sin(frog.zRotation) * cos(frog.yRotation) * frog.speed;

    state.currVel.x = state.initVel.x;
    state.currVel.y = state.initVel.y;
    state.currVel.z = state.initVel.z;

    state.go = false;
    state.startTime = 0.0;
}

void calcInitParams() {
    // ##### Grid variables #####
    grid.start = -grid.size;
    grid.end = grid.size;
    grid.range = grid.end - grid.start;

    // ##### Frog variables #####
    frog.step_theta = 2.0 * M_PI / global.segments;
    frog.step_phi = M_PI / global.segments;

    if (global.debug)
        printf(" - step_theta: %f\n - step_phi: %f\n", frog.step_theta, frog.step_phi);
}

void calcPosition(float dt){
    // Position
    state.currPos.x += state.currVel.x * dt;
    state.currPos.y += state.currVel.y * dt;
    state.currPos.z += state.currVel.z * dt;

    // Velocity
    state.currVel.y += g * dt;
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

void drawVector(float startX, float startY, float startZ, // Start coordinates
    float endX, float endY, float endZ,       // End coordinates
    float sizeScale, bool normalize,          // Whether scaled
    float red, float green, float blue)       // Color
{   // [Usage:] For drawing parabola tangents
    if (normalize) {
        float magnitude;
        magnitude = sqrt((endX * endX) + (endY * endY) + (endZ * endZ));
        endX /= magnitude;      endY /= magnitude;      endZ /= magnitude;
    }

    // Scale vector size then shift position to next strip
    endX = (endX * sizeScale) + startX;         
    endY = (endY * sizeScale) + startY;     
    endZ = (endZ * sizeScale) + startZ;

    drawLineStrip(startX, startY, startZ,
                  endX, endY, endZ,
                  red, green, blue);
}

void drawVelocityVector() {
    // Calculating end point
    float x, y, z;
    x = sin(frog.zRotation) * sin(frog.yRotation) * frog.speed;
    y = cos(frog.zRotation) * frog.speed;
    z = sin(frog.zRotation) * cos(frog.yRotation) * frog.speed;

    // Scale size
    x *= frog.velocityVectorSize;       y *= frog.velocityVectorSize;

    // Attach velocity vector to circle position
    x += state.currPos.x;
    y += state.currPos.y;
    z += state.currPos.z;

    drawLineStrip(state.currPos.x, state.currPos.y, 0.0,
                  x, y, 0.0,
                  1.0, 0.0, 1.0);
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

void drawGrid() {
    const float nSize = 0.5;

    // Ambient, diffuse and specular material colors
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);

    // Set lighting and material of shape, then draw
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Ambient, diffuse and specular material colors
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, gray);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, green);

    if (global.wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    float step = grid.range / global.segments;
    for (float i = grid.start; i < grid.end; i += step) {
        glBegin(GL_TRIANGLE_STRIP);
        for (float j = grid.start; j <= grid.end; j += step) {
            glVertex3f(i, 0.0, j);
            glVertex3f(i + step, 0.0, j);
        }
        glEnd();

        if (global.normals) {
            for (float j = grid.start; j <= grid.end; j += step) {
                drawNormal(0.5, i, 0.0, j);
                drawNormal(0.5, i + step, 0.0, j);
            }
        }
    }

    // Ensure lighting only applies to grid
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
}

void drawParabola() {
    // ##### Note: This is drawn via parametric method #####
    glColor3f(0.0, 0.0, 1.0); // Set color to blue
    
    // Update velocity based on rotation + speed
    state.initVel.x = sin(frog.zRotation) * sin(frog.yRotation) * frog.speed;
    state.initVel.y = cos(frog.zRotation) * frog.speed;
    state.initVel.z = sin(frog.zRotation) * cos(frog.yRotation) * frog.speed;

    float tof = (state.initVel.y * 2) / -g; // tof = time of flight
    float stepSize = tof / global.segments;
    float t, x, y, z;

    float endX, endY, endZ; // Used for drawing tangents

    glBegin(GL_LINE_STRIP);
        for (float i = 0; i <= global.segments; i++) {
            t = i * stepSize;

            x = state.initVel.x * t;                       // x = v*t
            y = (state.initVel.y * t) + (0.5 * g * t * t); // y = v*t - 1/2*g*t^2
            z = state.initVel.z * t;

            x += state.initPos.x;       
            y += state.initPos.y;
            z += state.initPos.z;

            glVertex3f(x, y, z);
        }
    glEnd();

    // Attach tangents
    if (global.normals) {
        for (float i = 0; i <= global.segments; i++) {
            // original coordinates of parabola
            t = i * stepSize;

            x = state.initVel.x * t;                       // x = v*t
            y = (state.initVel.y * t) + (0.5 * g * t * t); // y = v*t - 1/2*g*t^2
            z = state.initVel.z * t;

            // derivative of above equations
            endX = state.initVel.x;
            endY = state.initVel.y + (g * t);
            endZ = state.initVel.z;

            x += state.initPos.x;
            y += state.initPos.y;
            z += state.initPos.z;

            drawVector(x, y, z, endX, endY, endZ, frog.tangentSize, true, 0.0, 1.0, 1.0);
        }
    }
}

void drawFrog() {
    drawAxis(1.0);

    // In this case stacks and slices are equal
    float slices = global.segments, stacks = global.segments;
    float theta, phi;
    float x1, y1, z1,       x2, y2, z2,
          x3, y3, z3,       x4, y4, z4;

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

            // Adding current position to enable movement
            x1 += state.currPos.x;      y1 += state.currPos.y;      z1 += state.currPos.z;
            x2 += state.currPos.x;      y2 += state.currPos.y;      z2 += state.currPos.z;
            x3 += state.currPos.x;      y3 += state.currPos.y;      z3 += state.currPos.z;
            x4 += state.currPos.x;      y4 += state.currPos.y;      z4 += state.currPos.z;

            glBegin(GL_QUAD_STRIP);
                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);
                glVertex3f(x3, y3, z3);
                glVertex3f(x4, y4, z4);
            glEnd();

            //if (global.normals) {
            //    drawNormal(0.001, x1, y4, z1);
            //    drawNormal(0.001, x2, y4, z2);
            //    drawNormal(0.001, x3, y4, z3);
            //    drawNormal(0.001, x4, y4, z4);
            //}
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
        camera.currRotationY += dX;
        camera.currRotationX += dY;
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
}

// Keyboard functions
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        // Axes
        case 'o':
            global.axes = !global.axes;
            break;
        // Normal
        case 'n':
            global.normals = !global.normals;
            break;
        // Wireframe
        case 'p':
            global.wireframe = !global.wireframe;
            break;

        // Lighting
        case 'l':               
            break;
        // Textures
        case 't':
            break;

        // Speed increasing/decreasing
        case 'w':
            frog.speed += frog.speedChange;
            break;
        case 's':
            if (frog.speed >= frog.minSpeed)
                frog.speed -= frog.speedChange;
            break;

        // Angle increasing/decreasing
        case 'a':
            if (frog.zRotation <= degreesToRadians(frog.maxZRotation))
                frog.zRotation += degreesToRadians(1.0);
            break;
        case 'd':
            if (frog.zRotation >= degreesToRadians(frog.minZRotation))
                frog.zRotation -= degreesToRadians(1.0);
            break;

        // Jump key
        case ' ':
            state.currVel = state.initVel;
            if (!global.go) {
                state.startTime = glutGet(GLUT_ELAPSED_TIME) / (float)milli;
                state.go = true;
            }
            break;

        // Doubling/Halving segments (Grid, Sphere, Parabola, Cylinder)
        case '+':
            changeTesselation(true);
            break;
        case '-':
            changeTesselation(false);
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

void keyboardArrowKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        frog.yRotation += degreesToRadians(1.0);
        printf("yRotation: %f\n", frog.yRotation);
        break;
    case GLUT_KEY_RIGHT:
        frog.yRotation -= degreesToRadians(1.0);
        printf("yRotation: %f\n", frog.yRotation);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

// Reshaping, updating window/display
void update(void) {
    static float lastT = -1.0;
    float t, dt;

    if (!state.go)
        return;

    t = glutGet(GLUT_ELAPSED_TIME) / milli - state.startTime;

    if (lastT < 0.0) {
        lastT = t;
        return;
    }

    dt = t - lastT;
    calcPosition(dt);
    lastT = t;

    // Prevent frog from falling through
    if (state.currPos.y < 0.0) {
        state.go = false;
        state.currPos.y = 0.0;

        // Set starting currPosition to equal where frog jumped
        // Match velocity back to default
        state.initPos = state.currPos;
        state.currVel = state.initVel;
        lastT = -1.0;
    }

    glutPostRedisplay();
}

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
        glRotatef(camera.currRotationY, 0.0, 1.0, 0.0);
        glRotatef(camera.currRotationX, 1.0, 0.0, 0.0);

        // Drawing world axis
        drawAxis(2.0);

        drawGrid();
        drawFrog();
        drawParabola();
        //drawVelocityVector();

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
    glutSpecialFunc(keyboardArrowKeys);
    glutReshapeFunc(reshape);
    glutIdleFunc(update);
	glutMainLoop();

	return EXIT_SUCCESS;
}