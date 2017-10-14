#pragma once

#include "util.h"
#include "player.h"

// Launcher at top of level
const float LAUNCHER_LENGTH = 0.04;
const float LAUNCHER_WIDTH = 0.035;
const float LAUNCHER_SEGMENTS = 8.0;
const float LAUNCHER_RADIUS = 0.08;

// Catcher at bottom of level
const float CATCHER_SIZE = 1.0;
const float CATCHER_SPEED = -0.5;
const float CATCHER_HEIGHT = 0.075;
const float CATCHER_BUMPER_START = 0.125;
const float CATCHER_BUMPER_END = 0.2;
const float CATCHER_REBOUND = -1.0;

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
    Launcher(void) {};
    void init_launcher(void);
    void draw_launcher(Player *);
};

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
    void init_catcher(void);
    void draw_catcher(void);
    void move_catcher(float);
    void catcher_collide(Player *);
    bool caught_player(Player *);
};
