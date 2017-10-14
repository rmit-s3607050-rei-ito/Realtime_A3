
#include "machines.h"

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

void Launcher::draw_launcher(Player *player) {
  glPushMatrix();
    glTranslatef(0.0, PLAYER_START_POS_Y, 0.0); // Move to top of level

    // Drawing launcher
    glPushMatrix();
      glRotatef(-STARTING_ROTATION, 0.0, 0.0, 1.0); // Cannon points downwards

      // Drawing cannon
      glPushMatrix();
        glRotatef(player->rotation, 0.0, 0.0, 1.0);  // Rotate with player rotation
        glTranslatef(0.0, - (LAUNCHER_LENGTH + LAUNCHER_LENGTH), 0.0);
        drawSquare(cannon_top_l, cannon_top_r, cannon_bot_r, cannon_bot_l, cannon_color);
      glPopMatrix();

      // Drawing base
      glPushMatrix();
        setColoringMethod(base_color);
        drawCircle(segments, radius);
      glPopMatrix();
    glPopMatrix();

  glPopMatrix();
}

// Catcher related functions
void Catcher::init_catcher(void) {
  // Set default speed, moving leftwards
  velocity = CATCHER_SPEED;
  // Set how tall catcher is and size scaling
  height = CATCHER_HEIGHT;
  size = { CATCHER_SIZE, CATCHER_SIZE, CATCHER_SIZE };

  // Set positions for catcher left and right components
  // 1. Left section
  left_top_l = { -CATCHER_BUMPER_END, height };
  left_top_r = { -CATCHER_BUMPER_START, height };
  left_bottom_l = { -CATCHER_BUMPER_END, 0.0 };
  left_bottom_r = { -CATCHER_BUMPER_START, 0.0 };
  // 2. Right section
  right_top_l = { CATCHER_BUMPER_START, height };
  right_top_r = { CATCHER_BUMPER_END, height };
  right_bottom_l = { CATCHER_BUMPER_START, 0.0 };
  right_bottom_r = { CATCHER_BUMPER_END, 0.0 };

  // Set catcher to be at bottom of the level
  position.y = BOTTOM;
  collision_y = position.y + height;

  // Set colors for the catcher
  side_color = yellow;
  main_color = brown;
}

void Catcher::draw_catcher(void) {
  // Draws entire catcher part by part
  glPushMatrix();
    glTranslatef(position.x, position.y, 0.0);
    glScalef(size.x, size.y, size.z);
    // 1. Left side of catcher, collision detected
    drawSquare(left_top_l, left_top_r, left_bottom_r, left_bottom_l, side_color);
    // 3. Right side of catcher, collision detected
    drawSquare(right_top_l, right_top_r, right_bottom_r, right_bottom_l, side_color);
    // 2. Middle, enable falling through
    drawSquare(left_top_r, right_top_l, right_bottom_l, left_bottom_r, main_color);
  glPopMatrix();
}

// Movement and Collision
void Catcher::move_catcher(float dt) {
  float leftWall = LEFT - left_top_l.x;
  float rightWall = RIGHT - right_top_r.x;

  // Move catcher at constant rate
  position.x += dt * velocity;

  // When catcher collide with side of wall, flip direction
  if (position.x <= leftWall) {
    position.x += 2.0 * (leftWall - position.x);
    velocity *= CATCHER_REBOUND;
  }
  else if (position.x >= rightWall) {
    position.x += 2.0 * (rightWall - position.x);
    velocity *= CATCHER_REBOUND;
  }
}

void Catcher::catcher_collide(Player *player) {
  float bottomCollide = player->currPos.y - player->collisionRadius;

  float leftBumperStart = position.x + left_top_l.x;
  float leftBumperEnd = position.x + left_top_r.x;
  float rightBumperStart = position.x + right_top_l.x;
  float rightBumperEnd = position.x + right_top_r.x;

  printf("Left top l: %f,%f\n", left_top_l.x, left_top_l.y);

  float xPos = player->currPos.x;

  // Check whether ball reached bottom where catcher is
  if (bottomCollide <= collision_y) {
    // Check whether left or right bumper has been collided with
    if (xPos > leftBumperStart && xPos < leftBumperEnd) {
      player->currPos.y += 2.0 * (BOTTOM + height - bottomCollide);
      player->currVel.y *= -1.0;
    } else if (xPos > rightBumperStart && xPos < rightBumperEnd) {
      player->currPos.y += 2.0 * (BOTTOM + height - bottomCollide);
      player->currVel.y *= -1.0;
    }
  }
}

bool Catcher::caught_player(Player *player) {
  // Range for catcher, if land between these values ball is not lost
  float catcherStart = position.x + left_top_r.x;
  float catcherEnd = position.x + right_top_l.x;

  // Reduce life count by 1 if ball didnt end up in the catcher
  if (player->currPos.x < catcherStart || player->currPos.x > catcherEnd)
    return false;

  return true;
}
