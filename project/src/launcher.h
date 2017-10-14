#pragma once

#include "util.h"
#include "player.h"

// Launcher at top of level
const float LAUNCHER_LENGTH = 0.04;
const float LAUNCHER_WIDTH = 0.035;
const float LAUNCHER_SEGMENTS = 8.0;
const float LAUNCHER_RADIUS = 0.08;

class Launcher {
  // Cannon parameters
  float width;
  float length;
  glm::vec2 cannon_top_l, cannon_top_r, cannon_bot_l, cannon_bot_r;

  // Base parameters
  float segments;
  float radius;

  glm::vec3 cannon_color;
  glm::vec3 base_color;

  public:
    Launcher(void);
    void init_launcher(void);
    void draw_launcher(Player &);
};
