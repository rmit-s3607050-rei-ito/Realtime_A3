#include "globals.h"

// Initialization
void init_globals(Globals *globals) {
  globals->go = false;
  globals->wireframe = false;

  globals->dt = 0.0;
  globals->bounce = BOUNCE_FACTOR;
}

// Drawing the entire scene, level, player, walls, pegs, launcher and catcher
void draw_scene(Globals *globals) {
  // 1. Draw level walls
  globals->level.draw_level();
}
