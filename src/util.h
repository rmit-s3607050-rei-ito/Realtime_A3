// Utility file, contains constants and global variables
#pragma once

#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// Pi definition
#ifndef M_PI
  #define M_PI 4.0 * atan(1.0)
#endif

// Custom structs
typedef struct { float x, y, z; } vec3;
typedef struct { float x, y; } coordinate;
typedef struct { GLfloat r, g, b, a; } color4f;

// Global
typedef struct {
  bool go;
  bool wireframe;

  float elapsedTime;
  float startTime;
} Global;

// Constants
const float g = -9.8;         // Gravity
const float milli = 1000.0;   // One second

// Colors
const color4f white = { 1.0, 1.0, 1.0, 0.0 };
const color4f grey = { 0.8, 0.8, 0.8, 0.0 };
const color4f red = { 1.0, 0.0, 0.0, 0.0 };

// Window dimensions
const float WINDOW_X = 600.0;
const float WINDOW_Y = 700.0;
const float WINDOW_POS_X = 400.0;
const float WINDOW_POS_Y = 0.0;

// Functions
float degreesToRadians(float degrees);
void drawLineStrip(coordinate start, coordinate end, color4f color);

// ########### PLAYER RELATED STUFF ###########
// Player
typedef struct {
  coordinate initPos;
  coordinate currPos;

  // Changing launching angle
  float rotation;         // Around z axis
  float rotationInc;
  float minRotation;
  float maxRotation;

  float velocity;
  float radius;
  float mass;
  float elasticity;

  // Rendering params
  GLUquadric *quadric;
  int slices;
  int loops;
  vec3 size;
  color4f playerColor;
} Player;

void initPlayer(void);
void drawPlayer(void);

// ########## LEVEL RELATED STUFF ##########
// Level parameters
const coordinate topLeft  = { -0.90, 0.90 };
const coordinate topRight = { 0.90, 0.90 };
const coordinate botLeft  = { -0.90, -0.90 };
const coordinate botRight = { 0.90, -0.90 };

void drawLevelWindow(void);
