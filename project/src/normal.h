#pragma once

#include "peg.h"

const float CIRCLE_RADIUS = 0.06;
const int CIRCLE_SIDES = 16;

class Normal : public Peg
{
  public:
    Normal(type);

    // Overridden functions
    virtual void init_peg(void) override;
    virtual void draw_peg(void) override;
    virtual int peg_hit(void) override;
    virtual int peg_clear(void) override;
    virtual void init_vbo(void) override;
};
