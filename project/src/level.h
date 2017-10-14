#pragma once

#include "util.h"
// #include "player.h"
#include "machines.h"

const float WALL_GAP = 0.075;
const float WALL_REBOUND = -1.0;
const int NUM_BALLS = 8;

class Level {
  Player player;
  Launcher launcher;
  Catcher catcher;

  glm::vec2 top_left, top_right;
  glm::vec2 bot_left, bot_right;
  glm::vec3 wall_color;

  int balls;
  int score;

  public:
    Level(void) {};
    // void wall_collide(Player *);
    void init_level(void);
    void draw_level(void);

    int get_balls(void);
    int get_score(void);
};
