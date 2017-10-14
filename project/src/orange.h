#pragma once

#include "peg.h"

const glm::vec3 DEFAULT_COLOR = { 1.0, 0.5, 0.0 };
const glm::vec3 HIT_COLOR = { 1.0, 1.0, 0.0 };

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
    Normal(void);
    // Overridden functions
    virtual void init_peg(void) override;
    virtual void draw_peg(bool) override;
    int hit(void);
    int clear(void);
};
