#include "level.h"

void Level::init_level(void) {
  top_left  = { LEFT, TOP };
  top_right = { RIGHT, TOP };
  bot_left  = { LEFT, BOTTOM + WALL_GAP };
  bot_right = { RIGHT, BOTTOM + WALL_GAP };

  wall_color = white;

  balls = NUM_BALLS;
  score = 0;

  player.init_peg();
  launcher.init_launcher();
  catcher.init_catcher();
}

void Level::draw_level(void) {
  /* 1. Draw 3 lines forming top and sides of game level, can collide with these
   * a. Line from Top Left -> Top Right */
  drawLineStrip(top_left, top_right, wall_color);
  // b. Line from Top Right -> Bottom Right
  drawLineStrip(top_right, bot_right, wall_color);
  // c. Line from Top Left -> Bottom Left
  drawLineStrip(top_left, bot_left, wall_color);

  if (balls > 0) {
    player.draw_peg();

    // Draw pegs
  }

  // 3. Draw the launcher at the top
  launcher.draw_launcher(&player);

  // 4. Draw the catcher at the bottom
  catcher.draw_catcher();
}

// void Level::wall_collide(Player *player) {
//   glm::vec2 currPos = player->get_curr_pos();
//
//   float leftCollide = currPos.x - player->collision_radius;
//   float rightCollide = currPos.x + player->collision_radius;
//   float topCollide = currPos.y + player->collision_radius;
//
//   // Based on tutorial 10 wall collision
//   // Left wall hit
//   if (leftCollide <= LEFT) {
//     player->curr_pos.x += 2.0 * (LEFT - leftCollide);
//     player->curr_vel.x *= WALL_REBOUND;
//   }
//   else if (rightCollide >= RIGHT) {
//   // Right wall hit
//     player->curr_pos.x += 2.0 * (RIGHT - rightCollide);
//     player->curr_vel.x *= WALL_REBOUND;
//   }
//   // Top wall hit - Can collide with both TOP + LEFT/RIGHT (corner)
//   if (topCollide >= TOP) {
//     player->curr_pos.y += 2.0 * (TOP - topCollide);
//     player->curr_vel.y *= WALL_REBOUND;
//   }
// }

int Level::get_balls(void) {
  return balls;
}

int Level::get_score(void) {
  return score;
}
