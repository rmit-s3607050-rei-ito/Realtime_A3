#include "globals.h"

// Initialization
void init_globals(Globals *globals) {
  globals->level.init_level();
  globals->launcher.init_launcher();
  globals->catcher.init_catcher();

  initPlayer(&globals->player);

  globals->go = false;
  globals->wireframe = false;

  globals->dt = 0.0;
  globals->bounce = BOUNCE_FACTOR;

  globals->balls = NUM_BALLS;
  globals->score = 0;
}

// Drawing the entire scene, level, player, walls, pegs, launcher and catcher
void draw_scene(Globals *globals) {
  // 1. Draw level walls
  globals->level.draw_level();

  // 2. Draw player and trajectory prediction / guide
  if (globals->balls > 0) {
    drawPlayer(&globals->player);

    // if (!globals.go)
    drawGuide(&globals->player);
  }

  // 3. Draw the launcher at the top
  globals->launcher.draw_launcher(&globals->player);

  // 4. Draw the catcher at the bottom
  globals->catcher.draw_catcher();
}
