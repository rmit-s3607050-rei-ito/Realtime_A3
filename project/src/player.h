#pragma once

#include "util.h"

const float LAUNCH_POWER = 1.2;

typedef struct Player Player;

struct Player {
  // Positioning and velocity
  vec2 initPos, currPos;
  vec2 initVel, currVel;

  // Changing launching angle + displaying guide
  float rotation;         // Around z axis
  float minRotation, maxRotation;
  float rotationInc;
  float guideSize;
  float guideSegments;

  // Strength of projectile launch
  float power;

  float segments;
  float radius;
  float mass;
  float elasticity;

  vec3 size;
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
