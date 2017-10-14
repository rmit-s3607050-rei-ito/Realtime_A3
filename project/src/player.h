#pragma once

#include "util.h"

const float LAUNCH_POWER = 1.2;
const float PLAYER_SIZE = 0.5;

const float MIN_ROTATION = -180.0;
const float MAX_ROTATION = 0.0;
const float ROTATION_INCREMENT = 1.0;

const float GUIDE_SIZE = 5.0;
const float GUIDE_SEGMENTS = 16.0;

const float PLAYER_SEGMENTS = 8.0;
const float PLAYER_RADIUS = 0.05;
const float PLAYER_MASS = 1.0;
const float PLAYER_ELASTICITY = 1.0;

typedef struct Player Player;

struct Player {
  // Positioning and velocity
  glm::vec2 initPos, currPos;
  glm::vec2 initVel, currVel;
  glm::vec3 size;

  // Changing launching angle + displaying guide
  float rotation;         // Around z axis
  float minRotation, maxRotation;
  float rotationInc;
  float guideSize;
  float guideSegments;

  // Strength of projectile launch
  float power;

  // Collision detection
  float collisionRadius;
  float segments;
  float radius;
  float mass;
  float elasticity;

  color4f color;
};

// Initialization
void initPlayer(Player *);
// Drawing
void drawGuide(Player *);
void drawPlayer(Player *);
// Launch and movement
void setLaunch(Player *);
void integrate(float, Player *);
