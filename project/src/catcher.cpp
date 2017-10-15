#include "catcher.h"

Catcher::Catcher(void) {}

// #################### VBO related functionality ####################
void Catcher::init_vbo(void)
{
  // Allocate memory for indices and verticies
  // side_vbo.verts = (Vertex *) calloc(num_verts, sizeof(Vertex));
  // side_vbo.inds = (unsigned int*) calloc(num_inds, sizeof(int));

  // Coordinates for catcher sides
  // side_vbo.verts[0].pos = left_top_l;
  // side_vbo.verts[1].pos = left_top_r;
  // side_vbo.verts[2].pos = left_bottom_l;
  // side_vbo.verts[3].pos = left_bottom_r;
  // side_vbo.verts[4].pos = right_top_l;
  // side_vbo.verts[5].pos = right_top_r;
  // side_vbo.verts[6].pos = right_bottom_l;
  // side_vbo.verts[7].pos = right_bottom_r;
  // Coordinates for middle of catcher
  // mid_vbo.verts
  // Colors for each coordinate
  // verts[0].color = side_color;
  // verts[1].color = main_color;
  // verts[2].color = side_color;
  // verts[3].color = main_color;
  // verts[4].color = main_color;
  // verts[5].color = side_color;
  // verts[6].color = main_color;
  // verts[7].color = side_color;

  // Indices to draw points for catcher
  // Left bumper
  // side_vbo.inds[0] = 0;
  // side_vbo.inds[1] = 1;
  // side_vbo.inds[2] = 3;
  // side_vbo.inds[3] = 2;
  // Middle catcher
  // side_vbo.inds[4] = 1;
  // side_vbo.inds[5] = 4;
  // side_vbo.inds[6] = 6;
  // side_vbo.inds[7] = 3;
  // Right bumper
  // side_vbo.inds[8] = 4;
  // side_vbo.inds[9] = 5;
  // side_vbo.inds[10] = 7;
  // side_vbo.inds[11] = 6;
}

void Catcher::bind_vbo(void)
{
  // Store data for both vertices and indices in the VBO
  // Verticies
  // glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // glBufferData(GL_ARRAY_BUFFER, num_verts * sizeof(Vertex),
  //              verts, GL_STATIC_DRAW);
  //
  // // Indices
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_inds * sizeof(unsigned int),
  //              inds, GL_STATIC_DRAW);
}

// #################### Drawing and movement ####################
void Catcher::init_catcher(void)
{
  // Generate buffers for verticies and indices
  // glGenBuffers(1, &vbo);
  // glGenBuffers(1, &ibo);

  // Allocate number of verticies and indices for VBO
  // num_verts = CATCHER_NUM_VERTICES;
  // num_inds = CATCHER_NUM_INDICES;

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

  // Initialize and bind VBOs for usage
  // init_vbo();
  // bind_vbo();
}

void Catcher::draw_catcher(void)
{
  // Draws entire catcher part by part (immediate mode)
  glPushMatrix();
    glTranslatef(position.x, position.y, 0.0);
    glScalef(size.x, size.y, size.z);
    // 1. Left side of catcher, collision detected
    drawSquare(left_top_l, left_top_r, left_bottom_r, left_bottom_l, side_color);
    // 2. Right side of catcher, collision detected
    drawSquare(right_top_l, right_top_r, right_bottom_r, right_bottom_l, side_color);
    // 3. Middle, enable falling through
    drawSquare(left_top_r, right_top_l, right_bottom_l, left_bottom_r, main_color);
  glPopMatrix();

  // Draws entire catcher via VBOs using verticies/indices stored
  // glPushMatrix();
  //   setColoringMethod(main_color);
  //   glTranslatef(position.x, position.y, 0.0);
  //   glScalef(size.x, size.y, size.z);
  //
  //   // Enable pointers to vertex/colors and bind the current buffers to use
  //   glEnableClientState(GL_VERTEX_ARRAY);
  //   glEnableClientState(GL_COLOR_ARRAY);
  //   glBindBuffer(GL_ARRAY_BUFFER, vbo);
  //   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  //
  //   glVertexPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
  //   glColorPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(sizeof(glm::vec2)));
  //   glDrawElements(GL_POLYGON, num_indices, GL_UNSIGNED_INT, 0);
  // glPopMatrix();
}

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

// #################### Collision Detection ####################
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
