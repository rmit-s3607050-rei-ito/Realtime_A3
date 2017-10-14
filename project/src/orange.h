#pragma once

#include "peg.h"

const glm::vec3 ORANGE = { 1.0, 0.5, 0.0 };
const glm::vec3 ORANGE_HIT = { 1.0, 1.0, 0.0 };

class Orange : public Peg
{
  glm::vec2 position;
  glm::vec2 velocity;
  float radius;
  float segments;
  float collision_radius;
  bool hit;
  bool clear;

  public:
    Orange(void);
    // Overridden functions
    virtual void init_peg(void) override;
    virtual void draw_peg(void) override;
    int peg_hit(void);
    int peg_clear(void);
};
