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

#include "triangle.h"

Triangle::Triangle(type t) : Peg(t)
{
  init_peg();
}

void Triangle::init_peg()
{
  // Generating vbo buffers
  generate_buffers(&peg, TRIANGLE_SIDES, TRIANGLE_SIDES);

  mass = MASS;
  elasticity = ELASTICITY;
  size = SIZE;

  radius = TRIANGLE_RADIUS;
  sides = TRIANGLE_SIDES;

  // Set color based on type
  if (peg_type == blue)
    color = BLUE;
  else
    color = ORANGE;

  position = { 0.0, 0.0 };
  velocity = { 0.1, 0.1 }; //allow triangle to move

  collision_radius = radius * size.x;
  hit = false;
  clear = false;

  // Initialization and binding of vbos
  init_vbo();
  bind_vbo();
}

void Triangle::draw_peg()
{
  if (!clear && !empty) {
    // Via vbos
    glPushMatrix();
      glTranslatef(position.x, position.y, 0.0);
      glScalef(size.x, size.y, size.z);
      draw_vbo_shape(&peg, GL_POLYGON, color);
    glPopMatrix();
  }
}

int Triangle::peg_hit()
{
  int score_update = 0;

  if (!hit) {
    if (peg_type == orange) {
      color = ORANGE_HIT;
      score_update = ORANGE_HIT_POINT;
    } else {
      color = BLUE_HIT;
      score_update = BLUE_HIT_POINT;
    }
  }

  hit = true;
  return score_update;
}

int Triangle::peg_clear()
{
  int score_update = 0;

  if (hit) {
    if (!clear) {
      if (peg_type == orange)
        score_update = ORANGE_CLEAR_POINT;
      else
        score_update = BLUE_CLEAR_POINT;
    }
    clear = true;
  }

  return score_update;
}

void Triangle::init_vbo(void)
{
  // Initialize overall vbo for a peg
  init_vbo_polygon(&peg, sides, radius, color);
}
