#pragma once

#include "peg.h"

const float PENTAGON_RADIUS = 0.07;
const int PENTAGON_SIDES = 5;

class Pentagon : public Peg
{
  public:
    Pentagon(type);

    // Overridden functions
    virtual void init_peg(void) override;
    virtual void draw_peg(void) override;
    virtual int peg_hit(void) override;
    virtual int peg_clear(void) override;
    virtual void init_vbo(void) override;
};
