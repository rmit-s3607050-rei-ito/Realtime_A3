#include "level.h"

// #################### VBOs for level walls ####################
void Level::init_vbo(void)
{
  // Allocate memory for indices and verticies
  wall_vbo.verts = (Vertex *) calloc(wall_vbo.num_verts, sizeof(Vertex));
  wall_vbo.inds = (unsigned int*) calloc(wall_vbo.num_inds, sizeof(int));

  // Store coordinates for wall
  wall_vbo.verts[0].pos = top_left;
  wall_vbo.verts[1].pos = top_right;
  wall_vbo.verts[2].pos = bot_left;
  wall_vbo.verts[3].pos = bot_right;
  // Store colors for each wall coordinate (wireframe mode only)
  wall_vbo.verts[0].color = wall_color;
  wall_vbo.verts[1].color = wall_color;
  wall_vbo.verts[2].color = wall_color;
  wall_vbo.verts[3].color = wall_color;

  // Store indices to access wall
  // Left wall
  wall_vbo.inds[0] = 2;
  wall_vbo.inds[1] = 0;
  // Top wall
  wall_vbo.inds[2] = 0;
  wall_vbo.inds[3] = 1;
  // Right wall
  wall_vbo.inds[4] = 1;
  wall_vbo.inds[5] = 3;
}

void Level::bind_vbo()
{
  // Store data for both vertices and indices in the VBO
  // Verticies
  glBindBuffer(GL_ARRAY_BUFFER, wall_vbo.vbo);
  glBufferData(GL_ARRAY_BUFFER, wall_vbo.num_verts * sizeof(Vertex),
               wall_vbo.verts, GL_STATIC_DRAW);

  // Indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wall_vbo.ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, wall_vbo.num_inds * sizeof(unsigned int),
               wall_vbo.inds, GL_STATIC_DRAW);
}

// void Level::unbind_vbo(void)
// {
//   // Disable enabled client states
//   glDisableClientState(GL_VERTEX_ARRAY);
//   glDisableClientState(GL_COLOR_ARRAY);
//
//   // Free memory allocated to indices and verticies
//   free(wall_vbo.inds);
//   free(wall_vbo.verts);
//
//   // Empty buffers
//   glBindBuffer(GL_ARRAY_BUFFER, 0);
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
// }

void Level::init_pegs()
{
  float leftLimit = LEFT+0.1;
  float rightLimit = RIGHT;
  float bottomLimit = BOTTOM+0.35;
  float topLimit = TOP;

  float xInterval = (fabs(leftLimit-rightLimit))/WIDTH;
  float xCurr = leftLimit;

  float yInterval = (fabs(bottomLimit-topLimit))/HEIGHT;
  float yCurr = bottomLimit;

  oranges = 0;
  for(int row = 0; row < HEIGHT; row++) {
    for (int col = 0; col < WIDTH; col++) {
      pegs[row][col] = new Normal();
      if(((col + row) % 2) == 0) {
        pegs[row][col]->set_position(xCurr, yCurr);
        if (row % 2 == 0) {
          pegs[row][col]->set_orange();
          oranges++;
        }
      } else {
        pegs[row][col]->set_empty();
      }
      xCurr+=xInterval;
    }
    xCurr = leftLimit;
    yCurr+=yInterval;
  }
}

// ########## Level functionality ##########
void Level::init_level()
{
  // Initialize all level objects
  player.init_peg();
  launcher.init_launcher();
  catcher.init_catcher();
  init_pegs();

  // Generate buffers for verticies and indices
  glGenBuffers(1, &wall_vbo.vbo);
  glGenBuffers(1, &wall_vbo.ibo);

  // Allocate number of verticies and indices for VBO
  wall_vbo.num_verts = LEVEL_NUM_VERTICES;
  wall_vbo.num_inds = LEVEL_NUM_INDICES;

  // Assign position of walls and color
  top_left  = { LEFT, TOP };
  top_right = { RIGHT, TOP };
  bot_left  = { LEFT, BOTTOM + WALL_GAP };
  bot_right = { RIGHT, BOTTOM + WALL_GAP };
  wall_color = white;

  // Give player starting lives (balls) and starting score
  balls = NUM_BALLS;
  score = 0;

  // Create and bind vbos
  init_vbo();
  bind_vbo();
}

void Level::draw_walls(void)
{
  // 1. Draw 3 lines forming top and sides of game level (immediate mode)
  // drawLineStrip(top_left, top_right, wall_color);  // a. Top Left -> Top Right
  // drawLineStrip(top_right, bot_right, wall_color); // b. Top Right -> Bottom Right
  // drawLineStrip(top_left, bot_left, wall_color);   // c. Top Left -> Bottom Left

  // Via using VBOs
  glPushMatrix();
    setColoringMethod(wall_color);
    // Enable pointers to vertex/color coordinate arrays, bind current buffers
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, wall_vbo.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wall_vbo.ibo);

    glVertexPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
    glColorPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(sizeof(glm::vec2)));
    glDrawElements(GL_LINE_STRIP, wall_vbo.num_inds, GL_UNSIGNED_INT, 0);
  glPopMatrix();
}

void Level::draw_pegs()
{
  for(int row = 0; row < HEIGHT; row++) {
    for (int col = 0; col < WIDTH; col++) {
      pegs[row][col]->draw_peg();
    }
  }
}

void Level::draw_level()
{
  // 1. Draw walls using stored VBO data
  draw_walls();

  // 2. Draw player and trajectory guide
  if (balls > 0) { // Only when there are balls left to launch
    player.draw_peg();
    player.draw_guide();
  } else if (oranges == player.get_oranges_dest()) {
    printf("win\n");
  } else {
    printf("loose\n");
  }

  // 3. Draw all pegs
  draw_pegs();

  // 4. Draw the launcher at the top, collidable
  launcher.draw_launcher(player);

  // 5. Draw the catcher at the bottom, collidable
  catcher.draw_catcher();
}

bool Level::reset_player(void)
{
  float yPos = player.get_curr_pos().y;

  // If player has fallen through the bottom of the level
  if (yPos < BOTTOM && balls > 0) {
    // 1. Clear pegs that have been hit
    for(int row = 0; row < HEIGHT; row++) {
      for (int col = 0; col < WIDTH; col++) {
        score += pegs[row][col]->peg_clear();
      }
    }

    // 2. Reduce ball count only when player lands outside the catcher
    if (!catcher.caught_player(player))
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

  // Player collisions
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
