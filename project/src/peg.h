#pragma once

#include "util.h"

class peg
{
  protected:
    glm::vec2 position;
    glm::vec2 velocity;

    float mass;
    float elasticity;

    glm::vec3 size;
    glm::vec3 color;

    bool hit;
    bool clear;
  public:
    // Constructor
    peg(void);
    virtual void init_peg(void) = 0;
    virtual void draw_peg(bool) = 0;
    virtual int hit(void) = 0;
    virtual int clear(void) = 0;
};
