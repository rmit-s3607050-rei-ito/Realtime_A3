#pragma once

#include "peg.h"

class Normal : public Peg
{
  public:
    Normal(void);
    // Overridden functions
    virtual void init_peg(void) override;
    virtual void draw_peg(void) override;
    virtual int peg_hit(void) override;
    virtual int peg_clear(void) override;
};
