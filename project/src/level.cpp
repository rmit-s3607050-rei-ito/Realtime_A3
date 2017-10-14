
#include "level.h"

void initLevel(Level *level) {
  level->topLeft  = { LEFT, TOP };
  level->topRight = { RIGHT, TOP };
  level->botLeft  = { LEFT, BOTTOM + WALL_GAP };
  level->botRight = { RIGHT, BOTTOM + WALL_GAP };

  level->wallColor = white;
}

void drawLevel(Level *level) {
  /* 1. Draw 3 lines forming top and sides of game level, can collide with these
   * a. Line from Top Left -> Top Right */
  drawLineStrip(level->topLeft, level->topRight, level->wallColor);
  // b. Line from Top Right -> Bottom Right
  drawLineStrip(level->topRight, level->botRight, level->wallColor);
  // c. Line from Top Left -> Bottom Left
  drawLineStrip(level->topLeft, level->botLeft, level->wallColor);
}

void wallCollide(Player *player) {
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
