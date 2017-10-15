#pragma once

#include "peg.h"

const float SEGMENTS = 8.0;
const float RADIUS = 0.05;

const int BLUE_HIT_POINT = 1;
const int ORANGE_HIT_POINT = 2;
const int BLUE_CLEAR_POINT = 2;
const int ORANGE_CLEAR_POINT = 4;

class Normal : public Peg
{
  float radius;
  float segments;

  public:
    Normal(type);

    // Overridden functions
    virtual void init_peg(void) override;
    virtual void draw_peg(void) override;
    virtual int peg_hit(void) override;
    virtual int peg_clear(void) override;
    virtual void init_vbo(void) override;
};
