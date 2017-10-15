/* COSC1226 Real-Time Rendering and 3D Games Programming - Assignment 3
 * Assignment done as a pair
 * -----------------------------------------------------------------------------
 * Name             : Rei Ito
 * Student Number   : s3607050
 * -----------------------------------------------------------------------------
 * Name             : Pacific Thai
 * Student Number   : s3429648
 * -----------------------------------------------------------------------------
 */

#pragma once

#include "util.h"
#include "player.h"

// Launcher at top of level
const float LAUNCHER_LENGTH = 0.04;
const float LAUNCHER_WIDTH = 0.035;
const float LAUNCHER_SEGMENTS = 8.0;
const float LAUNCHER_RADIUS = 0.08;
// VBO params for base
const float BASE_NUM_VERTICES = 16.0;   // Double of launcher segments
const float BASE_NUM_INDICES = 16.0;    // All unique, equal to vertex order

class Launcher {
  // Cannon parameters
  float width;
  float length;
  glm::vec2 cannon_top_l, cannon_top_r, cannon_bottom_l, cannon_bottom_r;

  // VBOs
  Buffer cannon;
  Buffer base;

  // Base parameters
  float segments;
  float radius;

  glm::vec3 cannon_color;
  glm::vec3 base_color;

  public:
    Launcher(void);
    // VBOS
    void init_vbos(void);
    void bind_vbos(void);
    // void unbind_vbos(void);

    // Initialization and Drawing
    void init_launcher(void);
    void draw_launcher(Player &);
};
