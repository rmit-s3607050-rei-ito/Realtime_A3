#include "catcher.h"

Catcher::Catcher(void) {}

// Catcher related functions
void Catcher::init_catcher(void)
{
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

void Catcher::draw_catcher(void)
{
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
void Catcher::move_catcher(float dt)
{
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

bool Catcher::caught_player(Player & player)
{
  glm::vec2 playerPos = player.get_curr_pos();

  // Range for catcher, if land between these values ball is not lost
  float catcherStart = position.x + left_top_r.x;
  float catcherEnd = position.x + right_top_l.x;

  // Ball has not landed inside the catcher
  if (playerPos.x < catcherStart || playerPos.x > catcherEnd)
    return false;

  return true;
}

void Catcher::check_catcher_collision(Player * player) {
  // Get player's position and collidable radius
  glm::vec2 playerPos = player->get_curr_pos();
  float collisionRadius = player->get_collision_radius();

  // Bottom section of ball
  float bottomCollide = playerPos.y - collisionRadius;

  // Bumpers on catcher (yellow left and right sides)
  float leftBumperStart = position.x + left_top_l.x;
  float leftBumperEnd = position.x + left_top_r.x;
  float rightBumperStart = position.x + right_top_l.x;
  float rightBumperEnd = position.x + right_top_r.x;

  float displacement;

  // Check whether ball reached bottom where catcher is
  if (bottomCollide <= collision_y) {
    // Check whether left or right bumper has been collided with
    // if (playerPos.x > leftBumperStart && playerPos.x < leftBumperEnd) {
    //   displacement  = 2.0 * (BOTTOM + height - bottomCollide);
    //   player->rebound(Y_REFLECTION, displacement, CATCHER_REBOUND);
    // } else if (playerPos.x > rightBumperStart && playerPos.x < rightBumperEnd) {
    //   displacement  = 2.0 * (BOTTOM + height - bottomCollide);
    //   player->rebound(Y_REFLECTION, displacement, CATCHER_REBOUND);
    // }

    if ((playerPos.x > leftBumperStart && playerPos.x < leftBumperEnd) ||
        (playerPos.x > rightBumperStart && playerPos.x < rightBumperEnd))
    {
      displacement  = 2.0 * (BOTTOM + height - bottomCollide);
      player->rebound(Y_REFLECTION, displacement, CATCHER_REBOUND);
    }
  }
}
