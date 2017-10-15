#include "peg.h"

// #################### Constructor ####################
Peg::Peg(type t) : peg_type(t)
{
}

// #################### VBOs ####################
void Peg::bind_vbo(void)
{
  // Store data for both vertices and indices for player
  set_vbo_buffer_data(&peg);
}

// void Peg::unbind_vbo(void)
// {
//   clear_buffers(&peg);
// }

// #################### Getters ####################
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
  if (peg_type == orange)
    return true;

  return false;
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

// #################### Setters ####################
void Peg::set_position(float x, float y)
{
  position.x = x;
  position.y = y;
}

void Peg::set_empty()
{
  empty = true;
}
