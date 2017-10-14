#pragma once

#include "util.h"
#include "level.h"

typedef struct Globals Globals;

struct Globals {
  // All other class files
  Level level;

  // Global related variables
  bool go;
  bool wireframe;

  float dt;
  float bounce;
};

void init_globals(Globals *globals);
void draw_scene(Globals *globals);
