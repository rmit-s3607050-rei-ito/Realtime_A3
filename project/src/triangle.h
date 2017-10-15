#pragma once

#include "peg.h"

const float TRIANGLE_RADIUS = 0.08;
const int TRIANGLE_SIDES = 3;

class Triangle : public Peg
{
  public:
    Triangle(type);

    // Overridden functions
    virtual void init_peg(void) override;
    virtual void draw_peg(void) override;
    virtual int peg_hit(void) override;
    virtual int peg_clear(void) override;
    virtual void init_vbo(void) override;
};
