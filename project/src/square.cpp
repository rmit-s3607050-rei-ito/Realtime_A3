#include "square.h"

Square::Square(type t) : Peg(t)
{
  init_peg();
}

void Square::init_peg()
{
  // Generating vbo buffers
  generate_buffers(&peg, SQUARE_SIDES, SQUARE_SIDES);

  // Peg specific
  mass = MASS;
  elasticity = ELASTICITY;
  size = SIZE;

  radius = SQUARE_RADIUS;
  sides = SQUARE_SIDES;

  // Set color based on type
  if (peg_type == blue)
    color = BLUE;
  else
    color = ORANGE;

  // Normal specific
  position = { 0.0, 0.0 };
  velocity = { 0.0, 0.0 };

  collision_radius = radius * size.x;
  hit = false;
  clear = false;

  // Initialization and binding of vbos
  init_vbo();
  bind_vbo();
}

void Square::draw_peg()
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

int Square::peg_hit()
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

int Square::peg_clear()
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

void Square::init_vbo(void)
{
  // Initialize overall vbo for a peg
  init_vbo_polygon(&peg, sides, radius, color);
}
