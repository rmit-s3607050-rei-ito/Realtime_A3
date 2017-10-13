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

typedef enum { xCollide, yCollide } collision;

// Global
typedef struct {
  bool go;
  bool wireframe;

  float elapsedTime;
  float startTime;

  // Strength of bounce off the wall, should always be -ive
  float bounce;
  float minVelocity;

  int balls;
  int score;
} Global;

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

// Collision
const float COLLISION_SHIFT = 0.0001;

// Functions
float degreesToRadians(float);
void drawLineStrip(vec2, vec2, color4f);
void drawSquare(vec2, vec2, vec2, vec2, color4f);
void drawCircle(float, float);
void resetPlayer(void);

// ########### PLAYER RELATED STUFF ###########
// Player
typedef struct {
  // Positioning and velocity
  vec2 initPos, currPos;
  vec2 initVel, currVel;

  // Changing launching angle + displaying guide
  float rotation;         // Around z axis
  float minRotation, maxRotation;
  float rotationInc;
  float guideSize;
  float guideSegments;

  float segments;
  float radius;
  float mass;
  float elasticity;

  vec3 size;
  color4f color;
} Player;

void initPlayer(void);
void drawGuide(void);
void drawPlayer(void);

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

// ########## LEVEL RELATED STUFF ##########
// Level parameters
const float left = -0.90;
const float bottom = -1.0;
const float top = 0.90;
const float right = 0.90;
const vec2 topLeft  = { -0.90, 0.90 };
const vec2 topRight = { 0.90, 0.90 };
const vec2 botLeft  = { -0.90, -0.925 };
const vec2 botRight = { 0.90, -0.925 };

// Launcher at top of level
const float STARTING_ROTATION = -90.0;
const float LAUNCHER_LENGTH = 0.04;
const float LAUNCHER_WIDTH = 0.035;
const float LAUNCHER_POWER = 1.2;
const float LAUNCHER_SEGMENTS = 8.0;
const float LAUNCHER_RADIUS = 0.08;

// Catcher at bottom of level
const float CATCHER_SPEED = -0.5;
const float CATCHER_HEIGHT = 0.075;
const float CATCHER_BUMPER_START = 0.125;
const float CATCHER_BUMPER_END = 0.2;

typedef struct {
  // Cannon parameters
  float width;
  float length;
  vec2 cTopL, cTopR, cBotL, cBotR;

  // Strength of projectile launch
  float power;

  // Base parameters
  float segments;
  float radius;

} Launcher;

typedef struct {
  // Positioning
  vec2 position;
  vec2 leftTL, leftBL, leftTR, leftBR;
  vec2 rightTL, rightBL, rightTR, rightBR;

  // Collision detection
  float collisionY;

  // Movement
  float velocity;

  // Rendering params
  float height;
  vec3 size;
  color4f mainColor;
  color4f sideColor;

} Catcher;

void bruteForceWallCollide(float, float, float);
void bruteForceCatcherCollide(float, float);

void initLauncher(void);
void initCatcher(void);

void drawLauncher(void);
void drawCatcher(void);
void drawLevelWindow(void);

void moveCatcher(void);
