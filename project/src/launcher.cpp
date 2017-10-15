#include "launcher.h"

Launcher::Launcher(void) {}

// #################### VBOS ####################
void Launcher::init_vbos(void)
{
  // Initialize vbos for both cannon and base for launcher
  // 1. Cannon
  init_vbo_square(&cannon, cannon_top_l, cannon_top_r, cannon_bottom_r,
                  cannon_bottom_l, cannon_color);
  // 2. Base
  init_vbo_polygon(&base, segments, radius, base_color);
}

void Launcher::bind_vbos(void)
{
  // Store data for both vertices and indices in all VBOs:
  set_vbo_buffer_data(&base);
  set_vbo_buffer_data(&cannon);
}

// #################### Initialization and Drawing ####################
void Launcher::init_launcher(void) {
  // Base parameters
  generate_buffers(&base, BASE_NUM_VERTICES, BASE_NUM_INDICES);
  segments = LAUNCHER_SEGMENTS;
  radius = LAUNCHER_RADIUS;

  // Cannon parameters
  generate_buffers(&cannon, SQUARE_NUM_VERTICES, SQUARE_NUM_INDICES);
  length = LAUNCHER_LENGTH;
  width = LAUNCHER_WIDTH;

  // Positioning to draw shapes
  cannon_top_l = { -width, length };
  cannon_top_r = { width, length };
  cannon_bottom_l = { -width, -length };
  cannon_bottom_r = { width, -length };

  // Coloring
  cannon_color = GREY;
  base_color = DARK_GREY;

  // Initialize and bind vbos for usage
  init_vbos();
  bind_vbos();
}

void Launcher::draw_launcher(Player &player) {
  // Immediate mode
  // glPushMatrix();
  //   glTranslatef(0.0, PLAYER_START_POS_Y, 0.0); // Move to top of level
  //
  //   // Drawing launcher
  //   glPushMatrix();
  //     glRotatef(-STARTING_ROTATION, 0.0, 0.0, 1.0); // Cannon points downwards
  //
  //     // Drawing cannon
  //     glPushMatrix();
  //       glRotatef(player.get_rotation(), 0.0, 0.0, 1.0);  // Rotate with player rotation
  //       glTranslatef(0.0, - (LAUNCHER_LENGTH + LAUNCHER_LENGTH), 0.0);
  //       draw_square(cannon_top_l, cannon_top_r, cannon_bottom_r, cannon_bottom_l, cannon_color);
  //     glPopMatrix();
  //
  //     // Drawing base
  //     glPushMatrix();
  //       set_coloring_method(base_color);
  //       draw_circle(segments, radius);
  //     glPopMatrix();
  //   glPopMatrix();
  //
  // glPopMatrix();

  // Via using VBOs
  glPushMatrix();
    glTranslatef(0.0, PLAYER_START_POS_Y, 0.0); // Move to top of level

    // Drawing launcher
    glPushMatrix();
      glRotatef(-STARTING_ROTATION, 0.0, 0.0, 1.0); // Cannon points downwards

      // Drawing cannon
      glPushMatrix();
        glRotatef(player.get_rotation(), 0.0, 0.0, 1.0);  // Rotate with player rotation
        glTranslatef(0.0, - (LAUNCHER_LENGTH + LAUNCHER_LENGTH), 0.0);
        draw_vbo_shape(&cannon, GL_POLYGON, cannon_color);
      glPopMatrix();

      // Drawing base
      draw_vbo_shape(&base, GL_POLYGON, base_color);
    glPopMatrix();

  glPopMatrix();
}
