#pragma once

#include "util.h"
#include "player.h"

const float WALL_GAP = 0.075;
const float WALL_REBOUND = -1.0;

class Level {
  glm::vec2 top_left, top_right;
  glm::vec2 bot_left, bot_right;
  glm::vec3 wall_color;

  public:
    Level(void) { };
    void wall_collide(Player *);
    void init_level(void);
    void draw_level(void);
};
