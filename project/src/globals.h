#pragma once

#include "util.h"
#include "level.h"
#include "player.h"
#include "machines.h"

const float BOUNCE_FACTOR = -0.7;
const int NUM_BALLS = 8;

typedef struct Globals Globals;

struct Globals {
  // All other class files
  Level level;
  Player player;
  Launcher launcher;
  Catcher catcher;

  // Global related variables
  bool go;
  bool wireframe;

  float dt;
  float bounce;

  int balls;
  int score;
};

void initGlobals(Globals *globals);
void drawScene(Globals *globals);
