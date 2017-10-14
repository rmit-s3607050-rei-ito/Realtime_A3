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
}

void Normal::draw_peg(void)
{
  glPushMatrix();
    // if (type)
    //   glColor3fv(color);
    // else
    //   glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
    setColoringMethod(color);
    glTranslatef(position.x, position.y, 0.0);
    glScalef(size.x, size.y, size.z);
    drawCircle(segments, radius);
  glPopMatrix();
}

int Normal::peg_hit()
{
  hit = true;
  color = BLUE_HIT;
  return 1;
}

int Normal::peg_clear()
{
  if (hit)
    clear = true;
  return 5;
}
