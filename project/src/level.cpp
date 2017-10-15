#include "level.h"

// #################### VBOs for level walls ####################
void Level::init_vbo(void)
{
  // Allocate memory for indices and verticies
  wall.verts = (Vertex *) calloc(wall.num_verts, sizeof(Vertex));
  wall.inds = (unsigned int*) calloc(wall.num_inds, sizeof(int));

  // Store coordinates for wall
  wall.verts[0].pos = top_left;
  wall.verts[1].pos = top_right;
  wall.verts[2].pos = bot_left;
  wall.verts[3].pos = bot_right;

  // Store colors for each wall coordinate (wireframe mode only), all same color
  for (int i = 0; i < LEVEL_NUM_VERTICES; i++)
    wall.verts[i].color = wall_color;

  // Store indices to access wall
  // Left wall
  wall.inds[0] = 2;
  wall.inds[1] = 0;
  // Top wall
  wall.inds[2] = 0;
  wall.inds[3] = 1;
  // Right wall
  wall.inds[4] = 1;
  wall.inds[5] = 3;
}

void Level::bind_vbo(void)
{
  // Store data for both vertices and indices in the VBO
  set_vbo_buffer_data(&wall);
}

// void Level::unbind_vbo(void)
// {
//   clear_buffers(&wall);
// }

void Level::init_pegs(void)
{
  float leftLimit = LEFT+0.1;
  float rightLimit = RIGHT;
  float bottomLimit = BOTTOM+0.35;
  float topLimit = TOP;

  float xInterval = (fabs(leftLimit-rightLimit))/WIDTH;
  float xCurr = leftLimit;

  float yInterval = (fabs(bottomLimit-topLimit))/HEIGHT;
  float yCurr = bottomLimit;

  // Initialize peg positioning, add orange pegs per second row
  for(int row = 0; row < HEIGHT; row++) {
    for (int col = 0; col < WIDTH; col++) {
      pegs[row][col] = new Normal(blue);
      if(((col + row) % 2) == 0) {
        pegs[row][col]->set_position(xCurr, yCurr);
        if (row % 2 == 0) {
          pegs[row][col] = new Normal(orange);
          pegs[row][col]->set_position(xCurr, yCurr);
          num_orange_pegs++;
        }
      } else {
        pegs[row][col]->set_empty();
      }
      xCurr += xInterval;
    }
    xCurr = leftLimit;
    yCurr += yInterval;
  }
}

// ########## Level functionality ##########
void Level::init_level(void)
{
  // Generate buffers for verticies and indices for the wall
  generate_buffers(&wall, LEVEL_NUM_VERTICES, LEVEL_NUM_INDICES);

  // Assign position of walls and color
  top_left  = { LEFT, TOP };
  top_right = { RIGHT, TOP };
  bot_left  = { LEFT, BOTTOM + WALL_GAP };
  bot_right = { RIGHT, BOTTOM + WALL_GAP };
  wall_color = WHITE;

  // Give player starting lives (balls), no starting score and no orange pegs
  balls = NUM_BALLS;
  score = 0;
  num_orange_pegs = 0;

  // Initialize all level objects
  player.init_player();
  launcher.init_launcher();
  catcher.init_catcher();
  init_pegs();

  // Create and bind vbos
  init_vbo();
  bind_vbo();
}

void Level::draw_walls(void)
{
  // Draw 3 lines forming top and sides of game level (immediate mode)
  // draw_line_strip(top_left, top_right, wall_color);  // a. Top Left -> Top Right
  // draw_line_strip(top_right, bot_right, wall_color); // b. Top Right -> Bottom Right
  // draw_line_strip(top_left, bot_left, wall_color);   // c. Top Left -> Bottom Left

  // Via using VBOs
  draw_vbo_shape(&wall, GL_LINE_STRIP, wall_color);
}

void Level::draw_pegs(void)
{
  for(int row = 0; row < HEIGHT; row++) {
    for (int col = 0; col < WIDTH; col++) {
      pegs[row][col]->draw_peg();
    }
  }
}

void Level::draw_level(void)
{
  // 1. Draw walls
  draw_walls();

  // 2. Draw player and trajectory guide
  if (balls > 0) { // Only when there are balls left to launch
    player.draw_player();
    player.draw_guide();
  }

  // 3. Draw all pegs
  draw_pegs();

  // 4. Draw the launcher at the top, collidable
  launcher.draw_launcher(player);

  // 5. Draw the catcher at the bottom, collidable
  catcher.draw_catcher();
}

void Level::clear_hit_pegs(void){
  int scoreUpdate = 0;

  for(int row = 0; row < HEIGHT; row++) {
    for (int col = 0; col < WIDTH; col++) {
      scoreUpdate = pegs[row][col]->peg_clear();
      score += scoreUpdate;

      // Check if a orange peg was hit, if so update count left
      if(scoreUpdate == ORANGE_CLEAR_SCORE)
        num_orange_pegs--;
    }
  }
}

bool Level::reset_player(void)
{
  float yPos = player.get_curr_pos().y;

  // If player has fallen through the bottom of the level
  if (yPos < BOTTOM && balls > 0) {
    // 1. Clear pegs that have been hit
    clear_hit_pegs();

    // 2. Increase score when catacher caught player else reduce ball count
    if (catcher.caught_player(player))
      score += CATCHER_CATCH_SCORE;
    else
      balls--;

    // 3. Move player back to the top with initial values
    player.reset();

    return true;
  }

  return false;
}

// #################### Collision detection ####################
void Level::check_all_collisions(void)
{
  // Colisions against level walls: left, right and top
  check_wall_collision();

  // Collisions against sides of catcher: left, right
  catcher.check_catcher_collision(&player);

  // Player collisions with pegs
  check_peg_collisions();
}

void Level::check_peg_collisions(void)
{
  // Brute force checking peg by peg
  for(int row = 0; row < HEIGHT; row++) {
    for (int col = 0; col < WIDTH; col++) {
      if (player.peg_collide(pegs[row][col])) {
        score += pegs[row][col]->peg_hit();
        player.peg_collide_reflect(pegs[row][col]);
      }
    }
  }
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

  if (leftCollide <= LEFT) {
    // Left wall hit
    displacement = 2.0 * (LEFT - leftCollide);
    player.rebound(X_REFLECTION, displacement, WALL_REBOUND);
  }
  else if (rightCollide >= RIGHT) {
    // Right wall hit
    displacement = 2.0 * (RIGHT - rightCollide);
    player.rebound(X_REFLECTION, displacement, WALL_REBOUND);
  }
  if (topCollide >= TOP) {
    // Top wall hit - Can collide with both TOP + LEFT/RIGHT (corner)
    displacement = 2.0 * (TOP - topCollide);
    player.rebound(Y_REFLECTION, displacement, WALL_REBOUND);
  }
}

// #################### Getters ####################
int Level::get_balls(void)
{
  return balls;
}

int Level::get_score(void)
{
  return score;
}

// #################### Functions from attached classes ####################
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
