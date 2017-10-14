#include "normal.h"

Orange(void)
{
  init_peg();
}

void Orange::init_peg()
{
  // Peg specific
  mass = MASS;
  elasticity = ELASTICITY;
  size = SIZE;
  color = DEFAULT_COLOR;

  // Normal specific
  position = 0.0;
  velocity = 0.0;
  radius = RADIUS;
  segments = SEGMENTS;
  collision_radius = radius * size.x;
  hit = false;
  clear = false;
}

void Orange::draw_peg(bool type)
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

int Orange::hit()
{
  hit = true;
  color = HIT_COLOR;
  return 2;
}

int Orange::clear()
{
  if (hit)
    clear = true;
  return 10;
}
