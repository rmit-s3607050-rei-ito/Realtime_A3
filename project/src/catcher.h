#pragma once

#include "util.h"
#include "player.h"

// Catcher at bottom of level
const int CATCHER_NUM_VERTICES = 8; // 4 per side x 2 sides, middle uses shared verts
const int CATCHER_SIDE_INDICES = 8; // 4 per square, x 2 squares: left + right
const int CATCHER_MID_INDICES = 4;  // 4 for middle square
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

  // VBOs
  Buffer side_vbo;
  Buffer mid_vbo;

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
    // VBO related functions
    void init_vbo(void);
    void bind_vbo(void);
    // void unbind_vbos(void);

    // Drawing and movement
    void init_catcher(void);
    void draw_catcher(void);
    void move_catcher(float);

    // Collision detection
    bool caught_player(Player &);
    void check_catcher_collision(Player *);
};
