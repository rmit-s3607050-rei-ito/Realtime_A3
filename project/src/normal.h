#pragma once

#include "peg.h"

class normal : public peg
{
  float radius;
  float segments;

  public:
    normal(void);
    // Overridden functions
    virtual void init_peg(void) override;
    virtual void draw_peg(bool) override;
    virtual int hit(void) override;
    virtual int clear(void) override;
};
