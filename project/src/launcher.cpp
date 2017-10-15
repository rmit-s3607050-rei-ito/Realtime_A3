#include "launcher.h"

Launcher::Launcher(void) {}

// Launcher related functions
void Launcher::init_launcher(void) {
  // Base parameters
  segments = LAUNCHER_SEGMENTS;
  radius = LAUNCHER_RADIUS;

  // Cannon parameters
  length = LAUNCHER_LENGTH;
  width = LAUNCHER_WIDTH;

  // Positioning to draw shapes
  cannon_top_l = { -width, length };
  cannon_top_r = { width, length };
  cannon_bot_l = { -width, -length };
  cannon_bot_r = { width, -length };

  // Coloring
  cannon_color = grey;
  base_color = darkGrey;
}

void Launcher::draw_launcher(Player &player) {
  glPushMatrix();
    glTranslatef(0.0, PLAYER_START_POS_Y, 0.0); // Move to top of level

    // Drawing launcher
    glPushMatrix();
      glRotatef(-STARTING_ROTATION, 0.0, 0.0, 1.0); // Cannon points downwards

      // Drawing cannon
      glPushMatrix();
        glRotatef(player.get_rotation(), 0.0, 0.0, 1.0);  // Rotate with player rotation
        glTranslatef(0.0, - (LAUNCHER_LENGTH + LAUNCHER_LENGTH), 0.0);
        drawSquare(cannon_top_l, cannon_top_r, cannon_bot_r, cannon_bot_l, cannon_color);
      glPopMatrix();

      // Drawing base
      glPushMatrix();
        set_coloring_method(base_color);
        drawCircle(segments, radius);
      glPopMatrix();
    glPopMatrix();

  glPopMatrix();
}
