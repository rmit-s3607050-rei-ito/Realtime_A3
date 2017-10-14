#pragma once

#include "peg.h"

const glm::vec3 BLUE = { 0.0, 0.0, 1.0 };
const glm::vec3 BLUE_HIT = { 0.0, 1.0, 1.0 };

class Normal : public Peg
{
  glm::vec2 position;
  glm::vec2 velocity;
  float radius;
  float segments;
  float collision_radius;
  bool hit;
  bool clear;

  public:
    Normal(void);
    // Overridden functions
    virtual void init_peg(void) override;
    virtual void draw_peg(void) override;
    int peg_hit(void);
    int peg_clear(void);
};
