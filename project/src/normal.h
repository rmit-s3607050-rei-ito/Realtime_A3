#pragma once

#include "peg.h"

const glm::vec3 BLUE = { 0.0, 0.0, 1.0 };
const glm::vec3 BLUE_HIT = { 0.0, 1.0, 1.0 };
const glm::vec3 ORANGE = { 1.0, 0.5, 0.0 };
const glm::vec3 ORANGE_HIT = { 1.0, 1.0, 0.0 };

class Normal : public Peg
{
  glm::vec2 position;
  glm::vec2 velocity;
  float radius;
  float segments;
  float collision_radius;
  bool hit;
  bool clear;
  bool empty;
  bool orange;

  public:
    Normal(void);
    // Overridden functions
    virtual void init_peg(void) override;
    virtual void draw_peg(void) override;
    int peg_hit(void);
    int peg_clear(void);
    bool is_hit(void);
    bool is_clear(void);
    bool is_empty(void);
    bool is_orange(void);
    glm::vec2 get_position(void);
    glm::vec2 get_velocity(void);
    float get_collision_radius(void);
    void set_position(float, float);
    void set_empty(void);
    void set_orange(void);
};
