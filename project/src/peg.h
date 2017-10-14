#pragma once

#include "util.h"

const float MASS = 1.0;
const float ELASTICITY = 1.0;
const glm::vec3 SIZE = { 0.7, 0.7, 0.7 };

const float SEGMENTS = 8.0;
const float RADIUS = 0.05;

class Peg
{
  protected:
    float mass;
    float elasticity;
    glm::vec3 size;
    glm::vec3 color;

  public:
    // Constructor
    Peg(void);
    virtual void init_peg(void) = 0;
    virtual void draw_peg(void) = 0;
};
