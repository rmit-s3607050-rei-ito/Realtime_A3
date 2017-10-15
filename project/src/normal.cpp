#include "normal.h"

Normal::Normal(type t) : Peg(t)
{
  init_peg();
}

void Normal::init_peg()
{
  // Generating vbo buffers
  generate_buffers(&peg, PEG_NUM_VERTICES, PEG_NUM_INDICES);

  // Peg specific
  mass = MASS;
  elasticity = ELASTICITY;
  size = SIZE;

  // Set color based on type
  if (peg_type == blue)
    color = BLUE;
  else
    color = ORANGE;

  // Normal specific
  position = { 0.0, 0.0 };
  velocity = { 0.0, 0.0 };
  radius = RADIUS;
  segments = SEGMENTS;
  collision_radius = radius * size.x;
  hit = false;
  clear = false;

  // Initialization and binding of vbos
  init_vbo();
  bind_vbo();
}

void Normal::draw_peg()
{
  if (!clear && !empty) {
    // Immediate mode
    // glPushMatrix();
    //   set_coloring_method(color);
    //   glTranslatef(position.x, position.y, 0.0);
    //   glScalef(size.x, size.y, size.z);
    //   draw_circle(segments, radius);
    // glPopMatrix();

    // Via vbos
    glPushMatrix();
      glTranslatef(position.x, position.y, 0.0);
      glScalef(size.x, size.y, size.z);
      draw_vbo_shape(&peg, GL_POLYGON, color);
    glPopMatrix();
  }
}

int Normal::peg_hit()
{
  int score_update = 0;

  if (!hit) {
    if (peg_type == orange) {
      color = ORANGE_HIT;
      score_update = 2;
    } else {
      color = BLUE_HIT;
      score_update = 1;
    }
  }

  hit = true;
  return score_update;
}

int Normal::peg_clear()
{
  int score_update = 0;

  if (hit) {
    if (!clear) {
      if (peg_type == orange)
        score_update = 10;
      else
        score_update = 5;
    }

    clear = true;
  }

  return score_update;
}

void Normal::init_vbo(void)
{
  // Initialize overall vbo for a peg
  init_vbo_circle(&peg, segments, radius, color);
}
