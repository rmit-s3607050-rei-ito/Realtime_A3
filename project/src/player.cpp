#include "player.h"

Player::Player(void)
{
}

// #################### VBOs ####################
void Player::init_vbo(void)
{
  // Initialize vbo for player
  init_vbo_polygon(&player, segments, radius, color);
}

void Player::bind_vbo(void)
{
  // Store data for both vertices and indices for player
  set_vbo_buffer_data(&player);
}

// #################### Initialization and Display ####################
void Player::init_player()
{
  // Generating vbo buffers
  generate_buffers(&player, PLAYER_NUM_VERTICES, PLAYER_NUM_INDICES);

  // Positioning
  init_pos = { 0.0, PLAYER_START_POS_Y };
  curr_pos = init_pos;
  init_vel = { 0.0, 0.0 };
  curr_vel = init_vel;

  // Player information
  mass = PLAYER_MASS;
  elasticity = PLAYER_ELASTICITY;
  size = PLAYER_SIZE;
  color = WHITE;

  // Rotations
  rotation = STARTING_ROTATION;
  min_rotation = MIN_ROTATION;
  max_rotation = MAX_ROTATION;
  rotation_inc = ROTATION_INCREMENT;

  // Trajectory guide
  guide_size = GUIDE_SIZE;
  guide_segments = GUIDE_SEGMENTS;

  // Launch strength
  power = LAUNCH_POWER;

  // Drawing + internal params
  segments = PLAYER_SEGMENTS;
  radius = PLAYER_RADIUS;

  // Collision
  collision_radius = radius * size.x;

  // Initialize and bind vbos for usage
  init_vbo();
  bind_vbo();
}

void Player::draw_player()
{
  // Immediate mode
  // glPushMatrix();
  //   set_coloring_method(color);
  //   glTranslatef(curr_pos.x, curr_pos.y, 0.0);
  //   glRotatef(rotation, 0.0, 0.0, 1.0);
  //   glScalef(size.x, size.y, size.z);
  //   draw_circle(segments, radius);
  // glPopMatrix();

  // Via VBOs
  glPushMatrix();
    glTranslatef(curr_pos.x, curr_pos.y, 0.0);
    glRotatef(rotation, 0.0, 0.0, 1.0);
    glScalef(size.x, size.y, size.z);
    draw_vbo_shape(&player, GL_POLYGON, color);
  glPopMatrix();
}

void Player::draw_guide()
{
  glm::vec2 pos = { 0.0, 0.0 };
  glm::vec2 vel;
  vel.x = cos(degrees_to_radians(rotation)) * power;
  vel.y = sin(degrees_to_radians(rotation)) * power;

  // Guide drawn as a parabola
  float tof = (vel.y * 2.0) / gravity;
  float stepSize = tof / guide_segments;
  float t;

  glPushMatrix();
    glTranslatef(init_pos.x, init_pos.y, 0.0);
    set_coloring_method(GUIDE_COLOR);
    glBegin(GL_LINE_STRIP);
      for (float i = 0; i <= guide_segments; i++) {
        t = i * stepSize;
        pos.x += t * vel.x;
        pos.y += t * vel.y + (0.5 * gravity * t * t);
        vel.y += gravity * t;

        glVertex3f(pos.x, pos.y, 0.0);
      }
    glEnd();
  glPopMatrix();
}

// #################### Launching / Movement ####################
void Player::rotate_launch(direction path)
{
  // Rotate leftwards - do so when greater than minimum rotation
  if (path == LEFTWARDS && rotation > min_rotation)
    rotation -= rotation_inc;
  else if (path == RIGHTWARDS && rotation < max_rotation) {
  // Rotate rightwards - do so when less than maximum rotation
    rotation += rotation_inc;
  }
}

void Player::set_launch()
{
  // Set current velocity upon pressing space to launch
  curr_vel.x = cos(degrees_to_radians(rotation)) * power;
  curr_vel.y = sin(degrees_to_radians(rotation)) * power;
}

void Player::integrate(float dt)
{
  // Uses analytical approach to movement calculations
  curr_pos.x += dt * curr_vel.x;
  curr_pos.y += dt * curr_vel.y + (0.5 * gravity * dt * dt);

  // Factor in gravity in ball movement
  curr_vel.y += gravity * dt;
}

void Player::reset()
{
  // Ball has fallen out, reset positioning
  curr_pos = init_pos;
  curr_vel = init_vel;
}

// #################### Collision detection ####################
void Player::rebound(reflection axis, float displacement, float reflectPower)
{
  // Displace and reflect player when colliding with walls or the catcher's sides
  if (axis == X_REFLECTION) {
    curr_pos.x += displacement;
    curr_vel.x *= reflectPower;
  } else if (axis == Y_REFLECTION) {
    curr_pos.y += displacement;
    curr_vel.y *= reflectPower;
  }
}

bool Player::peg_collide(Peg *peg)
{
  double radiusSum, radiusSumSqr, dissMagSqr;
  glm::vec2 diss;

  glm::vec2 pegPos = peg->get_position();

  if (!peg->is_clear() && !peg->is_empty()) {
    radiusSum = collision_radius + peg->get_collision_radius();
    radiusSumSqr = radiusSum * radiusSum;
    diss.x = pegPos.x - curr_pos.x;
    diss.y = pegPos.y - curr_pos.y;
    dissMagSqr = (diss.x * diss.x) + (diss.y * diss.y);
    if (dissMagSqr <= radiusSumSqr) {
      return true;
    }
  }
  return false;
}

void Player::peg_collide_reflect(Peg *peg)
{
  float n[2], t[2], n_mag;
  float v1_nt[2], v2_nt[2];
  float m1, m2, v1i, v2i, v1f, v2f;

  glm::vec2 pegPos = peg->get_position();
  glm::vec2 pegVel = peg->get_velocity();

  n[0] = pegPos.x - curr_pos.x;;
  n[1] = pegPos.y - curr_pos.y;;

  n_mag = sqrt(n[0] * n[0] + n[1] * n[1]);
  n[0] /= n_mag;
  n[1] /= n_mag;

  t[0] = -n[1];
  t[1] = n[0];

  v1_nt[0] = n[0] * curr_vel.x + n[1] * curr_vel.y;
  v1_nt[1] = t[0] * curr_vel.x + t[1] * curr_vel.y;
  v2_nt[0] = n[0] * pegVel.x + n[1] * pegVel.y;
  v2_nt[1] = t[0] * pegVel.x + t[1] * pegVel.y;

  m1 = mass;
  m2 = peg->get_mass();
  v1i = v1_nt[0];
  v2i = v2_nt[0];
  v1f = (m1 - m2) / (m1 + m2) * v1i + 2.0 * m2 / (m1 + m2) * v2i;
  v2f = 2.0 * m1 / (m1 + m2) * v1i + (m2 - m1) / (m1 + m2) * v2i;

  v1_nt[0] = v1f;
  v2_nt[0] = v2f;

  curr_vel.x = (n[0] * v1_nt[0] + t[0] * v1_nt[1])
    - (n[0] * v2_nt[0] + t[0] * v2_nt[1]);
  curr_vel.y = (n[1] * v1_nt[0] + t[1] * v1_nt[1])
    - (n[1] * v2_nt[0] + t[1] * v2_nt[1]);
}

// #################### Getters ####################
glm::vec2 Player::get_curr_pos()
{
  return curr_pos;
}

float Player::get_rotation()
{
  return rotation;
}

float Player::get_collision_radius()
{
  return collision_radius;
}
