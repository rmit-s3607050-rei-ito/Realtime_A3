#include "orange.h"

Orange::Orange(void)
{
  init_peg();
}

void Orange::init_peg()
{
  // Peg specific
  mass = MASS;
  elasticity = ELASTICITY;
  size = SIZE;
  color = ORANGE;

  // Normal specific
  position = { 0.0, 0.0 };
  velocity = { 0.0, 0.0 };
  radius = RADIUS;
  segments = SEGMENTS;
  collision_radius = radius * size.x;
  hit = false;
  clear = false;
}

void Orange::draw_peg(void)
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

int Orange::peg_hit()
{
  hit = true;
  color = ORANGE_HIT;
  return 2;
}

int Orange::peg_clear()
{
  if (hit)
    clear = true;
  return 10;
}
