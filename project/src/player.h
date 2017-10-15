#pragma once

#include "util.h"
#include "peg.h"

const float PLAYER_MASS = 1.0;
const float PLAYER_ELASTICITY = 1.0;

const glm::vec3 PLAYER_SIZE = {0.5, 0.5, 0.5 };
const float PLAYER_SEGMENTS = 8.0;
const float PLAYER_RADIUS = 0.05;

const float LAUNCH_POWER = 1.2;

const float MIN_ROTATION = -180.0;
const float MAX_ROTATION = 0.0;
const float ROTATION_INCREMENT = 1.0;

const float GUIDE_SIZE = 5.0;
const float GUIDE_SEGMENTS = 16.0;

const glm::vec3 WHITE = { 1.0, 1.0, 1.0 };
const glm::vec3 GUIDE_COLOR = { 1.0, 0.0, 0.0 };

class Player
{
  glm::vec2 init_pos, curr_pos;
  glm::vec2 init_vel, curr_vel;

  float mass;
  float elasticity;
  glm::vec3 size;
  glm::vec3 color;

  float rotation;
  float min_rotation, max_rotation;
  float rotation_inc;

  float guide_size;
  float guide_segments;

  float power;
  float radius;
  float segments;
  float collision_radius;

  int oranges_dest;

  public:
    Player(void);
    // Overridden functions
    void init_player(void);
    void draw_player(void);

    void draw_guide(void);
    void rotate_launch(direction);
    void set_launch(void);
    void integrate(float);
    void reset(void);

    // Collisions
    void rebound(reflection, float, float);

    // Getters
    glm::vec2 get_curr_pos(void);
    float get_rotation(void);
    float get_collision_radius(void);
    int get_oranges_dest(void);

    bool peg_collide(Peg *);
    void peg_collide_reflect(Peg *);
};
