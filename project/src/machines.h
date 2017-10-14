#pragma once

#include "util.h"
#include "player.h"

// Launcher at top of level
const float LAUNCHER_LENGTH = 0.04;
const float LAUNCHER_WIDTH = 0.035;
const float LAUNCHER_SEGMENTS = 8.0;
const float LAUNCHER_RADIUS = 0.08;

// Catcher at bottom of level
const float CATCHER_SIZE = 1.0;
const float CATCHER_SPEED = -0.5;
const float CATCHER_HEIGHT = 0.075;
const float CATCHER_BUMPER_START = 0.125;
const float CATCHER_BUMPER_END = 0.2;
const float CATCHER_REBOUND = -1.0;

typedef struct Launcher Launcher;
typedef struct Catcher Catcher;

struct Launcher {
  // Cannon parameters
  float width;
  float length;
  glm::vec2 cTopL, cTopR, cBotL, cBotR;

  // Base parameters
  float segments;
  float radius;

  glm::vec3 cannonColor;
  glm::vec3 baseColor;
};

struct Catcher {
  // Positioning
  glm::vec2 position;
  glm::vec2 leftTL, leftBL, leftTR, leftBR;
  glm::vec2 rightTL, rightBL, rightTR, rightBR;

  // Collision detection
  float collisionY;

  // Movement
  float velocity;

  // Rendering params
  float height;
  glm::vec3 size;
  glm::vec3 mainColor;
  glm::vec3 sideColor;
};

// Initialization
void initLauncher(Launcher *);
void initCatcher(Catcher *);
// Drawing
void drawLauncher(Launcher *, Player *);
void drawCatcher(Catcher *);
// Movement and Collision
void moveCatcher(Catcher *, float);
void catcherCollide(Catcher *, Player *);
bool caughtPlayer(Catcher *, Player *);
