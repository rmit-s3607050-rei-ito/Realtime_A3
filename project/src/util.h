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

// Rotating launch
enum reflection { X_REFLECTION, Y_REFLECTION };
enum direction { LEFTWARDS, RIGHTWARDS };

const float BOUNCE_FACTOR = -0.7;

// Constants
// const float gravity = -9.8;   // Gravity
const float gravity = -2.0;   // Fake Gravity
const float milli = 1000.0;   // One second

// Colors
const glm::vec3 white = { 1.0, 1.0, 1.0 };
const glm::vec3 grey = { 0.2, 0.2, 0.2 };
const glm::vec3 red = { 1.0, 0.0, 0.0 };
const glm::vec3 yellow = { 1.0, 1.0, 0.0 };
const glm::vec3 brown = { 0.55, 0.27, 0.07 };
const glm::vec3 darkGrey = { 0.1, 0.1, 0.1 };

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
void setColoringMethod(glm::vec3);
void drawLineStrip(glm::vec2, glm::vec2, glm::vec3);
void drawSquare(glm::vec2, glm::vec2, glm::vec2, glm::vec2, glm::vec3);
void drawCircle(float, float);

void resetPlayer(void);
