#pragma once

#include "util.h"
#include "player.h"

// Catcher at bottom of level
const float CATCHER_SIZE = 1.0;
const float CATCHER_SPEED = -0.5;
const float CATCHER_HEIGHT = 0.075;
const float CATCHER_BUMPER_START = 0.125;
const float CATCHER_BUMPER_END = 0.2;
const float CATCHER_REBOUND = -1.0;

class Catcher {
  // Positioning
  glm::vec2 position;
  glm::vec2 left_top_l, left_bottom_l, left_top_r, left_bottom_r;
  glm::vec2 right_top_l, right_bottom_l, right_top_r, right_bottom_r;

  // Collision detection
  float collision_y;

  // Movement
  float velocity;

  // Rendering params
  float height;
  glm::vec3 size;
  glm::vec3 main_color, side_color;

  public:
    Catcher(void);
    void init_catcher(void);
    void draw_catcher(void);
    void move_catcher(float);

    bool caught_player(Player &);
    void check_catcher_collision(Player *);
};
