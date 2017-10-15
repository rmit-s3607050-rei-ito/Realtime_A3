#pragma once

#include "util.h"
#include "peg.h"

const float PLAYER_MASS = 1.0;
const float PLAYER_ELASTICITY = 1.0;

const glm::vec3 PLAYER_SIZE = {0.5, 0.5, 0.5 };
const float PLAYER_SEGMENTS = 8.0;
const float PLAYER_RADIUS = 0.05;

const float PLAYER_NUM_VERTICES = 16.0;  // Should be double segment count
const float PLAYER_NUM_INDICES = 16.0;   // Same as vertex count

const float LAUNCH_POWER = 1.2;

const float MIN_ROTATION = -180.0;
const float MAX_ROTATION = 0.0;
const float ROTATION_INCREMENT = 1.0;

const float GUIDE_SIZE = 5.0;
const float GUIDE_SEGMENTS = 16.0;

const glm::vec3 GUIDE_COLOR = { 1.0, 0.0, 0.0 };

class Player
{
  // Positioning
  glm::vec2 init_pos, curr_pos;
  glm::vec2 init_vel, curr_vel;

  // VBOs
  Buffer player;

  // Drawing
  float radius;
  float segments;
  glm::vec3 size;
  glm::vec3 color;

  // Rotating for launch
  float rotation;
  float min_rotation, max_rotation;
  float rotation_inc;

  // Attached guide
  float guide_size;
  float guide_segments;

  // Collisions
  float power;
  float mass;
  float elasticity;
  float collision_radius;

  public:
    Player(void);
    // VBOs
    void init_vbo(void);
    void bind_vbo(void);
    // void unbind_vbo(void);

    // Initialization and Display
    void init_player(void);
    void draw_player(void);
    void draw_guide(void);

    // Launching / Movement and Trajectory
    void rotate_launch(direction);
    void set_launch(void);
    void integrate(float);
    void reset(void);

    // Collisions
    void rebound(reflection, float, float);
    bool peg_collide(Peg *);
    void peg_collide_reflect(Peg *);

    // Getters
    glm::vec2 get_curr_pos(void);
    float get_rotation(void);
    float get_collision_radius(void);
};
