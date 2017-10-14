#include "level.h"

// ########## VBOs for level walls ##########
void Level::init_vbo(void)
{
  // Generate buffers for verticies and indices
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ibo);

  // Allocate memory for indices and verticies
  verticies = (glm::vec2 *) calloc(num_vertices, sizeof(glm::vec2));
  indices = (unsigned int*) calloc(num_indices, sizeof(int));

  // Store coordinates for wall
  verticies[0] = top_left;
  verticies[1] = top_right;
  verticies[2] = bot_left;
  verticies[3] = bot_right;

  // Store indices to access wall
  // Left wall
  indices[0] = 2;
  indices[1] = 0;
  // Top wall
  indices[2] = 0;
  indices[3] = 1;
  // Right wall
  indices[4] = 1;
  indices[5] = 3;
}

void Level::bind_vbo(void)
{
  // Verticies
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(glm::vec2),
               verticies, GL_STATIC_DRAW);

  // Indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(unsigned int),
               indices, GL_STATIC_DRAW);

  // Enable pointer sot vertex coordinate arrays
  glEnableClientState(GL_VERTEX_ARRAY);
}

// void unbind_vbo(void)
// {
//   // Disable enabled client states
//   glDisableClientState(GL_VERTEX_ARRAY);
//
//   // Free memory allocated to indices and verticies
//   free(indices);
//   free(verticies);
//
//   // Empty buffers
//   glBindBuffer(GL_ARRAY_BUFFER, 0);
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
// }

// ########## Level functionality ##########
void Level::init_level(void)
{
  num_vertices = NUM_VERTICES;
  num_indices = NUM_INDICES;

  top_left  = { LEFT, TOP };
  top_right = { RIGHT, TOP };
  bot_left  = { LEFT, BOTTOM + WALL_GAP };
  bot_right = { RIGHT, BOTTOM + WALL_GAP };

  wall_color = white;

  balls = NUM_BALLS;
  score = 0;

  init_vbo();
  bind_vbo();

  player.init_peg();
  launcher.init_launcher();
  catcher.init_catcher();
}

void Level::draw_level(void)
{
  // 1. Draw walls using stored VBO data
  glPushMatrix();
    setColoringMethod(white);
    glVertexPointer(2, GL_FLOAT, sizeof(glm::vec2), BUFFER_OFFSET(0));
    glDrawElements(GL_LINE_STRIP, num_indices, GL_UNSIGNED_INT, 0);
  glPopMatrix();

  // 2. Draw player and trajectory guide
  if (balls > 0) { // Only when there are balls left to launch
    player.draw_peg();
    player.draw_guide();
  }

  // 3. Draw all pegs

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

// ########## Collision detection ##########
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

// ########## Getters ##########
int Level::get_balls(void)
{
  return balls;
}

int Level::get_score(void)
{
  return score;
}

// ########## Functions from attached classes ##########
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
