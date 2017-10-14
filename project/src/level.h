#pragma once

#include "util.h"
#include "player.h"

const float WALL_GAP = 0.075;
const float WALL_REBOUND = -1.0;

typedef struct Level Level;

struct Level {
  vec2 topLeft;
  vec2 topRight;
  vec2 botLeft;
  vec2 botRight;

  color4f wallColor;
};

void initLevel(Level *);
void drawLevel(Level *);
void wallCollide(Player *, float, float, float);
