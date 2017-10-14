// Utility file, contains constants and global variables
#pragma once

// SHADER STUFF
// #define GL_GLEXT_PROTOTYPES
// #include "shaders.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

#define GLM_FORCE_RADIANS
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

typedef enum { xCollide, yCollide } collision;

// Constants
// const float gravity = -9.8;   // Gravity
const float gravity = -2.0;   // Fake Gravity
const float milli = 1000.0;   // One second

// Colors
const color4f white = { 1.0, 1.0, 1.0, 0.0 };
const color4f grey = { 0.2, 0.2, 0.2, 0.0 };
const color4f red = { 1.0, 0.0, 0.0, 0.0 };
const color4f yellow = { 1.0, 1.0, 0.0, 0.0 };
const color4f brown = { 0.55, 0.27, 0.07, 0.0 };
const color4f darkGrey = { 0.1, 0.1, 0.1, 0.0 };

// Window dimensions
const float WINDOW_X = 600.0;
const float WINDOW_Y = 700.0;
const float WINDOW_POS_X = 400.0;
const float WINDOW_POS_Y = 0.0;

// Shared variables
const float LEFT = -0.90;
const float BOTTOM = -1.0;
const float TOP = 0.90;
const float RIGHT = 0.90;

const float PLAYER_START_POS_Y = 0.85;
const float STARTING_ROTATION = -90.0;

// Functions
float degreesToRadians(float);
void setColoringMethod(color4f color);
void drawLineStrip(vec2, vec2, color4f);
void drawSquare(vec2, vec2, vec2, vec2, color4f);
void drawCircle(float, float);
void resetPlayer(void);

// ######## OBSTACLE RELATED STUFF #########
const int WIDTH = 15;
const int HEIGHT = 6;

typedef struct {
  vec2 pos;
  vec2 vel;

  float radius;
  float mass;
  float elasticity;

  // Rendering params
  float segments;
  vec3 size;
  color4f color;

  bool hit;
  bool clear;
  bool empty;
} Obstacle;

void initObstacle(Obstacle *);
void initObstacles(void);
void drawObstacles(void);
