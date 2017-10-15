/* COSC1226 Real-Time Rendering and 3D Games Programming - Assignment 3
 * Assignment done as a pair
 * -----------------------------------------------------------------------------
 * Name             : Rei Ito
 * Student Number   : s3607050
 * -----------------------------------------------------------------------------
 * Name             : Pacific Thai
 * Student Number   : s3429648
 * -----------------------------------------------------------------------------
 */

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

// Peg animation
void Peg::move_peg(float dt)
{
  float leftWall = LEFT + radius;
  float rightWall = RIGHT - radius;

  // Move catcher at constant rate
  position.x += dt * velocity.y;

  // When catcher collide with side of wall, flip direction
  if (position.x <= leftWall) {
    position.x += 2.0 * (leftWall - position.x);
    velocity *= PEG_REBOUND;
  }
  else if (position.x >= rightWall) {
    position.x += 2.0 * (rightWall - position.x);
    velocity *= PEG_REBOUND;
  }
}

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
