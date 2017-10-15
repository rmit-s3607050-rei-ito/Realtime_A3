#pragma once

#include "peg.h"

const float SEGMENTS = 8.0;
const float RADIUS = 0.05;

class Normal : public Peg
{
  float radius;
  float segments;

  public:
    Normal(void);
    // Overridden functions
    virtual void init_peg(void) override;
    virtual void draw_peg(void) override;
    virtual int peg_hit(void) override;
    virtual int peg_clear(void) override;
};
