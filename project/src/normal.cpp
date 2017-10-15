#include "normal.h"

Normal::Normal(void)
{
  init_peg();
}

void Normal::init_peg()
{
  // Peg specific
  mass = MASS;
  elasticity = ELASTICITY;
  size = SIZE;
  color = BLUE;

  // Normal specific
  position = { 0.0, 0.0 };
  velocity = { 0.0, 0.0 };
  radius = RADIUS;
  segments = SEGMENTS;
  collision_radius = radius * size.x;
  hit = false;
  clear = false;
  orange = false;
}

void Normal::draw_peg()
{
  if (!clear && !empty) {
    glPushMatrix();
      set_coloring_method(color);
      glTranslatef(position.x, position.y, 0.0);
      glScalef(size.x, size.y, size.z);
      draw_circle(segments, radius);
    glPopMatrix();
  }
}

int Normal::peg_hit()
{
  int ret = 0;

  if (!hit) {
    if (orange) {
      color = ORANGE_HIT;
      ret = 2;
    } else {
      color = BLUE_HIT;
      ret = 1;
    }
  }

  hit = true;
  return ret;
}

int Normal::peg_clear()
{
  int ret = 0;

  if (hit) {
    if (!clear) {
      if (orange)
        ret = 10;
      else
        ret = 5;
    }

    clear = true;
  }

  return ret;
}
