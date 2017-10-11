// Utility file, contains constants and global variables
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

// Pi definition
#ifndef M_PI
  #define M_PI 4.0 * atan(1.0)
#endif

// Custom structs
typedef struct { float x, y, z; } vec3;
typedef struct { float x, y; } vec2;
typedef struct { GLfloat r, g, b, a; } color4f;

// Global
typedef struct {
  bool go;
  bool wireframe;

  float elapsedTime;
  float startTime;

  // Strength of bounce off the wall, should always be -ive
  float bounce;
  float minVelocity;

} Global;

// Constants
const float gravity = -9.8;   // Gravity
const float milli = 1000.0;   // One second

// Colors
const color4f white = { 1.0, 1.0, 1.0, 0.0 };
const color4f grey = { 0.8, 0.8, 0.8, 0.0 };
const color4f red = { 1.0, 0.0, 0.0, 0.0 };
// const color4f yellow = { 1.0, 1.0, 0.0, 0.0 };
const color4f brown = { 0.55, 0.27, 0.07, 0.0 };

// Window dimensions
const float WINDOW_X = 600.0;
const float WINDOW_Y = 700.0;
const float WINDOW_POS_X = 400.0;
const float WINDOW_POS_Y = 0.0;

// Functions
float degreesToRadians(float);
// float roundDownFloat(int);
void drawLineStrip(vec2, vec2, color4f);

// ########### PLAYER RELATED STUFF ###########
const int STARTING_LIVES = 5;

// Player
typedef struct {
  // Lives
  int numLives;

  // Positioning and velocity
  vec2 initPos, currPos;
  vec2 initVel, currVel;

  // Strength of projectile launch
  float power;

  // Changing launching angle + displaying guide
  float rotation;         // Around z axis
  float rotationInc;
  float minRotation, maxRotation;
  float guideSize;

  float radius;
  float mass;
  float elasticity;

  // Rendering params
  GLUquadric *quadric;
  int slices;
  int loops;
  vec3 size;
  color4f color;
} Player;

void initPlayer(void);
void drawPlayer(void);

// ######## OBSTACLE RELATED STUFF #########
typedef struct {
  vec2 pos;
  vec2 vel;

  float radius;
  float mass;
  float elasticity;

  // Rendering params
  GLUquadric *quadric;
  int slices;
  int loops;
  vec3 size;
  color4f color;

  bool hit;
} Obstacle;

void initObstacles(void);
void drawObstacles(void);

// ########## LEVEL RELATED STUFF ##########
// Level parameters
const float left = -0.90;
const float bottom = -0.90;
const float top = 0.90;
const float right = 0.90;
const vec2 topLeft  = { -0.90, 0.90 };
const vec2 topRight = { 0.90, 0.90 };
const vec2 botLeft  = { -0.90, -0.90 };
const vec2 botRight = { 0.90, -0.90 };

void bruteForceWallCollide(float, float, float);
void drawLevelWindow(void);

// Catcher at bottom of level
const float CATCHER_SPEED = -0.5;
const float CATCHER_HEIGHT = 0.04;

typedef struct {
  // Positioning
  vec2 position;
  vec2 leftStart, leftEnd;
  vec2 rightStart, rightEnd;

  // Movement
  float velocity;

  // Rendering params
  float height;
  vec3 size;
  color4f color;

} Catcher;

void bruteForceCatcherCollide(float, float, float);
void initCatcher(void);
void drawCatcher(void);
void moveCatcher(void);
