#include "catcher.h"

Catcher::Catcher(void) {}

// #################### VBO related functionality ####################
void Catcher::init_vbos(void)
{
  // Allocate memory for indices and verticies for both sides and middle
  left.verts = (Vertex *) calloc(left.num_verts, sizeof(Vertex));
  left.inds = (unsigned int*) calloc(left.num_inds, sizeof(int));

  right.verts = (Vertex *) calloc(right.num_verts, sizeof(Vertex));
  right.inds = (unsigned int*) calloc(right.num_inds, sizeof(int));

  middle.verts = (Vertex *) calloc(middle.num_verts, sizeof(Vertex));
  middle.inds = (unsigned int*) calloc(middle.num_inds, sizeof(int));

  // Coordinates for catcher sides
  // Left
  left.verts[0].pos = left_top_l;
  left.verts[1].pos = left_top_r;
  left.verts[2].pos = left_bottom_r;
  left.verts[3].pos = left_bottom_l;
  // Right
  right.verts[0].pos = right_top_l;
  right.verts[1].pos = right_top_r;
  right.verts[2].pos = right_bottom_r;
  right.verts[3].pos = right_bottom_l;
  // Middle
  middle.verts[0].pos = left_top_r;
  middle.verts[1].pos = right_top_l;
  middle.verts[2].pos = right_bottom_l;
  middle.verts[3].pos = left_bottom_r;

  // Colors for coordinates for sides (all same color)
  for (int i = 0; i < CATCHER_NUM_VERTICES; i++) {
    left.verts[i].color = side_color;
    right.verts[i].color = side_color;
    middle.verts[i].color = main_color;
  }

  // Indices to draw points. For all parts of the catcher, indices match coord order
  for (int i = 0; i < CATCHER_NUM_INDICES; i++) {
    right.inds[i] = i;
    left.inds[i] = i;
    middle.inds[i] = i;
  }
}

void Catcher::bind_vbos(void)
{
  // Store data for both vertices and indices in all VBOs:

  // 1. Left:
  // Verticies
  glBindBuffer(GL_ARRAY_BUFFER, left.vbo);
  glBufferData(GL_ARRAY_BUFFER, left.num_verts * sizeof(Vertex),
               left.verts, GL_STATIC_DRAW);
  // Indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, left.ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, left.num_inds * sizeof(unsigned int),
               left.inds, GL_STATIC_DRAW);

  // 2. Right:
  // Verticies
  glBindBuffer(GL_ARRAY_BUFFER, right.vbo);
  glBufferData(GL_ARRAY_BUFFER, right.num_verts * sizeof(Vertex),
              right.verts, GL_STATIC_DRAW);
  // Indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, right.ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, right.num_inds * sizeof(unsigned int),
              right.inds, GL_STATIC_DRAW);

  // 2. Middle:
  // Verticies
  glBindBuffer(GL_ARRAY_BUFFER, middle.vbo);
  glBufferData(GL_ARRAY_BUFFER, middle.num_verts * sizeof(Vertex),
              middle.verts, GL_STATIC_DRAW);

  // Indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, middle.ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, middle.num_inds * sizeof(unsigned int),
              middle.inds, GL_STATIC_DRAW);
}

// #################### Drawing and movement ####################
void Catcher::init_catcher(void)
{
  // Generate buffers for verticies and indices
  glGenBuffers(1, &left.vbo);         glGenBuffers(1, &left.ibo);
  glGenBuffers(1, &right.vbo);       glGenBuffers(1, &right.ibo);
  glGenBuffers(1, &middle.vbo);       glGenBuffers(1, &middle.ibo);

  // Allocate number of verticies and indices for both VBOs
  left.num_verts = CATCHER_NUM_VERTICES;
  left.num_inds = CATCHER_NUM_INDICES;
  right.num_verts = CATCHER_NUM_VERTICES;
  right.num_inds = CATCHER_NUM_INDICES;
  middle.num_verts = CATCHER_NUM_VERTICES;
  middle.num_inds = CATCHER_NUM_INDICES;

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
  init_vbos();
  bind_vbos();
}

void Catcher::draw_catcher(void)
{
  // Draws entire catcher part by part (immediate mode)
  // glPushMatrix();
  //   glTranslatef(position.x, position.y, 0.0);
  //   glScalef(size.x, size.y, size.z);
  //   // 1. Left side of catcher, collision detected
  //   drawSquare(left_top_l, left_top_r, left_bottom_r, left_bottom_l, side_color);
  //   // 2. Right side of catcher, collision detected
  //   drawSquare(right_top_l, right_top_r, right_bottom_r, right_bottom_l, side_color);
  //   // 3. Middle, enable falling through
  //   drawSquare(left_top_r, right_top_l, right_bottom_l, left_bottom_r, main_color);
  // glPopMatrix();

  // Draws entire catcher via VBOs using verticies/indices stored
  glPushMatrix();
    glTranslatef(position.x, position.y, 0.0);
    glScalef(size.x, size.y, size.z);

    // 1. Draw sides
    glPushMatrix();
      set_coloring_method(side_color);
      // LEFT
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);
      glBindBuffer(GL_ARRAY_BUFFER, left.vbo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, left.ibo);

      glVertexPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
      glColorPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(sizeof(glm::vec2)));
      glDrawElements(GL_POLYGON, left.num_inds, GL_UNSIGNED_INT, 0);
    glPopMatrix();
    glPushMatrix();
      // RIGHT
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);
      glBindBuffer(GL_ARRAY_BUFFER, right.vbo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, right.ibo);

      glVertexPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
      glColorPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(sizeof(glm::vec2)));
      glDrawElements(GL_POLYGON, right.num_inds, GL_UNSIGNED_INT, 0);
    glPopMatrix();

    // 2. Draw middle
    glPushMatrix();
      set_coloring_method(main_color);
      // Enable pointers to vertex/colors and bind the current buffers to use
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);
      glBindBuffer(GL_ARRAY_BUFFER, middle.vbo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, middle.ibo);

      glVertexPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
      glColorPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(sizeof(glm::vec2)));
      glDrawElements(GL_POLYGON, middle.num_inds, GL_UNSIGNED_INT, 0);
    glPopMatrix();
  glPopMatrix();
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
