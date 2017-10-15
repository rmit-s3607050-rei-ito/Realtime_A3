#pragma once

#include "peg.h"

const float HEXAGON_RADIUS = 0.07;
const int HEXAGON_SIDES = 6;

class Hexagon : public Peg
{
  public:
    Hexagon(type);

    // Overridden functions
    virtual void init_peg(void) override;
    virtual void draw_peg(void) override;
    virtual int peg_hit(void) override;
    virtual int peg_clear(void) override;
    virtual void init_vbo(void) override;
};
