#include "level.h"

// Main level functionality
void Level::init_level(void)
{
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

void Level::draw_level(void)
{
  /* 1. Draw 3 lines forming top and sides of game level, collidable
   * a. Line from Top Left -> Top Right */
  drawLineStrip(top_left, top_right, wall_color);
  // b. Line from Top Right -> Bottom Right
  drawLineStrip(top_right, bot_right, wall_color);
  // c. Line from Top Left -> Bottom Left
  drawLineStrip(top_left, bot_left, wall_color);

  // 2. Draw player and trajectory guide
  if (balls > 0) { // Only when there are balls left to launch
    player.draw_peg();
    player.draw_guide();
  }

  // Draw all pegs

  // 4. Draw the launcher at the top, collidable
  launcher.draw_launcher(player);

  // 5. Draw the catcher at the bottom, collidable
  catcher.draw_catcher();
}

bool Level::reset_player(void)
{
  float yPos = player.get_curr_pos().y;

  // If player has fallen through the bottom of the level
  if (yPos < BOTTOM) {
    // 1. Clear pegs that have been hit

    // 2. Reduce ball count only when player lands outside the catcher
    if (!catcher.caught_player(player))
      balls--;

    // 3. Move player back to the top with initial values
    player.reset();

    return true;
  }

  return false;
}

void Level::check_all_collisions(void)
{
  // Colisions against level walls: left, right and top
  check_wall_collision();

  // Collisions against sides of catcher: left, right
  catcher.check_catcher_collision(&player);
}

void Level::check_wall_collision(void)
{
  // Based on tutorial 10 wall collision

  // Get player's current position and radius
  glm::vec2 currPos = player.get_curr_pos();
  float collisionRadius = player.get_collision_radius();

  // Define collision check ranges
  float leftCollide = currPos.x - collisionRadius;
  float rightCollide = currPos.x + collisionRadius;
  float topCollide = currPos.y + collisionRadius;

  // Store small rebound upon collision to pass to player
  float displacement;

  // Left wall hit
  if (leftCollide <= LEFT) {
    displacement = 2.0 * (LEFT - leftCollide);
    player.rebound(X_REFLECTION, displacement, WALL_REBOUND);
  }
  else if (rightCollide >= RIGHT) {
  // Right wall hit
    displacement = 2.0 * (RIGHT - rightCollide);
    player.rebound(X_REFLECTION, displacement, WALL_REBOUND);
  }
  // Top wall hit - Can collide with both TOP + LEFT/RIGHT (corner)
  if (topCollide >= TOP) {
    displacement = 2.0 * (TOP - topCollide);
    player.rebound(Y_REFLECTION, displacement, WALL_REBOUND);
  }
}

int Level::get_balls(void)
{
  return balls;
}

int Level::get_score(void)
{
  return score;
}

// Calling functions from attached classes
// Catcher
void Level::move_catcher(float dt)
{
  catcher.move_catcher(dt);
}

// Player
void Level::rotate_launcher(direction path)
{
  player.rotate_launch(path);
}

void Level::set_launch(void)
{
  player.set_launch();
}

void Level::integrate(float dt)
{
  player.integrate(dt);
}
