
#include "level.h"

void Level::init_level(void) {
  top_left  = { LEFT, TOP };
  top_right = { RIGHT, TOP };
  bot_left  = { LEFT, BOTTOM + WALL_GAP };
  bot_right = { RIGHT, BOTTOM + WALL_GAP };

  wall_color = white;
}

void Level::draw_level(void) {
  /* 1. Draw 3 lines forming top and sides of game level, can collide with these
   * a. Line from Top Left -> Top Right */
  drawLineStrip(top_left, top_right, wall_color);
  // b. Line from Top Right -> Bottom Right
  drawLineStrip(top_right, bot_right, wall_color);
  // c. Line from Top Left -> Bottom Left
  drawLineStrip(top_left, bot_left, wall_color);
}

void Level::wall_collide(Player *player) {
  float leftCollide = player->currPos.x - player->collisionRadius;
  float rightCollide = player->currPos.x + player->collisionRadius;
  float topCollide = player->currPos.y + player->collisionRadius;

  // Based on tutorial 10 wall collision
  // Left wall hit
  if (leftCollide <= LEFT) {
    player->currPos.x += 2.0 * (LEFT - leftCollide);
    player->currVel.x *= WALL_REBOUND;
  }
  else if (rightCollide >= RIGHT) {
  // Right wall hit
    player->currPos.x += 2.0 * (RIGHT - rightCollide);
    player->currVel.x *= WALL_REBOUND;
  }
  // Top wall hit - Can collide with both TOP + LEFT/RIGHT (corner)
  if (topCollide >= TOP) {
    player->currPos.y += 2.0 * (TOP - topCollide);
    player->currVel.y *= WALL_REBOUND;
  }
}
