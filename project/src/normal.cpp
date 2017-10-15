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
      drawCircle(segments, radius);
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

bool Normal::is_hit()
{
  return hit;
}

bool Normal::is_clear()
{
  return clear;
}

bool Normal::is_empty()
{
  return empty;
}

bool Normal::is_orange()
{
  return orange;
}

glm::vec2 Normal::get_position()
{
  return position;
}

glm::vec2 Normal::get_velocity()
{
  return velocity;
}

float Normal::get_collision_radius()
{
  return collision_radius;
}

void Normal::set_position(float x, float y)
{
  position.x = x;
  position.y = y;
}

void Normal::set_empty()
{
  empty = true;
}

void Normal::set_orange()
{
  color = ORANGE;
  orange = true;
}
