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
  Launcher launcher;
  Player player;
  Catcher catcher;

  // Global related variables
  bool go;
  bool wireframe;

  float dt;
  float bounce;

  int balls;
  int score;
};

void init_globals(Globals *globals);
void draw_scene(Globals *globals);
