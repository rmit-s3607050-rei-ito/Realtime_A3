#include "globals.h"

// Initialization
void initGlobals(Globals *globals) {
  initLevel(&globals->level);
  initPlayer(&globals->player);
  initLauncher(&globals->launcher);
  initCatcher(&globals->catcher);

  globals->go = false;
  globals->wireframe = false;

  globals->dt = 0.0;
  globals->bounce = BOUNCE_FACTOR;

  globals->balls = NUM_BALLS;
  globals->score = 0;
}

// Drawing the entire scene, level, player, walls, pegs, launcher and catcher
void drawScene(Globals *globals) {
  // 1. Draw level walls
  drawLevel(&globals->level);

  // 2. Draw player and trajectory prediction / guide
  if (globals->balls > 0) {
    drawPlayer(&globals->player);

    // if (!globals.go)
    drawGuide(&globals->player);
  }

  // 3. Draw the launcher at the top
  drawLauncher(&globals->launcher, &globals->player);

  // 4. Draw the catcher at the bottom
  drawCatcher(&globals->catcher);
}
