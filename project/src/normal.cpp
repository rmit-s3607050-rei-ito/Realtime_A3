#include "normal.h"

normal(void)
{
  init_peg();
}

void init_peg()
{
  // peg specific
  position = { 0.0,0.0 };
  velocity = { 0.0, 0.0 };
  mass = 5.0;
  elasticity = 1.0;
  size = { 0.7, 0.7, 0.7 };
  color = { 0.0, 0.0, 1.0 };
  hit = false;
  clear = false;

  // normal specific
  radius = 0.05;
  segments = 8.0;
}

void draw_peg(bool type)
{
  glPushMatrix();
    if (type)
      glColor3fv(color);
    else
      glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
    glTranslatef(position.x, posistion.y, 0.0);
    glScalef(size.x, size.y, size.z);
    drawCircle(segments, radius);
  glPopMatrix();
}

int hit()
{
  hit = true;
  color = { 0.0, 1.0, 1.0 };
  return 1;
}

int clear()
{
  if (hit)
    clear = true;
  color = { 0.0, 0.0, 0.0 };
  return 5;
}
