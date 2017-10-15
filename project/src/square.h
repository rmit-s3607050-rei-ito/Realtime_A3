#pragma once

#include "peg.h"

const float SQUARE_RADIUS = 0.07;
const int SQUARE_SIDES = 4;

class Square : public Peg
{
  public:
    Square(type);

    // Overridden functions
    virtual void init_peg(void) override;
    virtual void draw_peg(void) override;
    virtual int peg_hit(void) override;
    virtual int peg_clear(void) override;
    virtual void init_vbo(void) override;
};
