#include "peg.h"

// Constructor
Peg::Peg(void)
{
}

bool Peg::is_hit()
{
  return hit;
}

bool Peg::is_clear()
{
  return clear;
}

bool Peg::is_empty()
{
  return empty;
}

bool Peg::is_orange()
{
  return orange;
}

glm::vec2 Peg::get_position()
{
  return position;
}

glm::vec2 Peg::get_velocity()
{
  return velocity;
}

float Peg::get_mass()
{
  return mass;
}

float Peg::get_collision_radius()
{
  return collision_radius;
}

void Peg::set_position(float x, float y)
{
  position.x = x;
  position.y = y;
}

void Peg::set_empty()
{
  empty = true;
}

void Peg::set_orange()
{
  color = ORANGE;
  orange = true;
}
