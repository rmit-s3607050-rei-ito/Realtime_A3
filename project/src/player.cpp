#include "player.h"

Player(void)
{
  init_peg();
}

void Player::init_peg()
{
  // Peg specific
  mass = MASS;
  elasticity = ELASTICITY;
  size = SIZE;
  color = DEFAULT_COLOR;

  // Player specific
  // Positioning
  init_pos = { 0.0, PLAYER_START_POS_Y };
  curr_pos = init_pos;
  init_vel = { 0.0, 0.0 };
  curr_vel = init_vel;

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
}

void Player::draw_peg(bool type)
{
  glPushMatrix();
    if (type)
      glColor3fv(color);
    else
      glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
    glTranslatef(curr_pos.x, curr_pos.y, 0.0);
    glRotatef(rotation, 0.0, 0.0, 1.0);
    glScalef(size.x, size.y, size.z);
    drawCircle(segments, radius);
    draw_guide(type;)
  glPopMatrix();
}

void Player::draw_guide(bool type)
{
  glm::vec2 pos = { 0.0, 0.0 };
  glm::vec2 vel;
  vel.x = cos(degreesToRadians(rotation)) * power;
  vel.y = sin(degreesToRadians(rotation)) * power;

  // Guide drawn as a parabola
  float tof = (vel.y * 2.0) / gravity;
  float stepSize = tof / guide_segments;
  float t;

  glPushMatrix();
    glTranslatef(0.0, PLAYER_START_POS_Y, 0.0); // Move to top of level
    if (type)
      glColor3fv(GUIDE_COLOR);
    else
      glMaterialfv(GL_FRONT, GL_DIFFUSE, GUIDE_COLOR);
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

  // setColoringMethod(red);
  // glBegin(GL_LINE_STRIP);
  //   for (int i = 0; i < numPoints; i++) {
  //     vec2 guide = calculateGuidePoints();
  //     // glVertex3f(prediction[i].x, prediction[i].y, 0.0);
  //     glVertex3f(guide.x, guide.y, 0.0);
  //   }
  // glEnd();
}

void Player::set_launch()
{
  curr_vel.x = cos(degreesToRadians(rotation)) * power;
  curr_vel.y = sin(degreesToRadians(rotation)) * power;
}

void Player::integrate(float dt)
{
  // Uses analytical approach to movement calculations
  curr_pos.x += dt * curr_vel.x;
  curr_pos.y += dt * curr_vel.y + (0.5 * gravity * dt * dt);

  // Factor in gravity in ball movement
  curr_vel.y += gravity * dt;
}

bool Player::peg_collide(Normal peg)
{
  double radiusSum, radiusSumSqr, dissMagSqr;
  glm::vec2 diss;

  if (!peg.clear && !peg.empty) {
    radiusSum = collision_radius + peg.collision_radius;
    radiusSumSqr = radiusSum * radiusSum;
    diss.x = peg.posistion.x - curr_pos.x;
    diss.y = peg.position.y - curr_pos.y;
    dissMagSqr = (diss.x * diss.x) + (diss.y * diss.y);
    if (dissMagSqr <= radiusSumSqr)
      return true;
  }

  return false;
}

bool Player::peg_collide(Orange peg)
{
  double radiusSum, radiusSumSqr, dissMagSqr;
  glm::vec2 diss;

  if (!peg.clear && !peg.empty) {
    radiusSum = collision_radius + peg.collision_radius;
    radiusSumSqr = radiusSum * radiusSum;
    diss.x = peg.posistion.x - curr_pos.x;
    diss.y = peg.position.y - curr_pos.y;
    dissMagSqr = (diss.x * diss.x) + (diss.y * diss.y);
    if (dissMagSqr <= radiusSumSqr)
      return true;
  }

  return false;
}

void Player::peg_collide_reflect(Normal peg)
{
  float n[2], t[2], n_mag;
  float v1_nt[2], v2_nt[2];
  float m1, m2, v1i, v2i, v1f, v2f;

  n[0] = peg.position.x - curr_pos.x;;
  n[1] = peg.position.y - curr_pos.y;;

  n_mag = sqrt(n[0] * n[0] + n[1] * n[1]);
  n[0] /= n_mag;
  n[1] /= n_mag;

  t[0] = -n[1];
  t[1] = n[0];

  v1_nt[0] = n[0] * curr_vel.x + n[1] * curr_vel.y;
  v1_nt[1] = t[0] * curr_vel.x + t[1] * curr_vel.y;
  v2_nt[0] = n[0] * peg.velocity.x + n[1] * peg.velocity.y;
  v2_nt[1] = t[0] * peg.velocity.x + t[1] * peg.velocity.y;

  m1 = mass;
  m2 = peg.mass;
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

void Player::peg_collide_reflect(Orange peg)
{
  float n[2], t[2], n_mag;
  float v1_nt[2], v2_nt[2];
  float m1, m2, v1i, v2i, v1f, v2f;

  n[0] = peg.position.x - curr_pos.x;;
  n[1] = peg.position.y - curr_pos.y;;

  n_mag = sqrt(n[0] * n[0] + n[1] * n[1]);
  n[0] /= n_mag;
  n[1] /= n_mag;

  t[0] = -n[1];
  t[1] = n[0];

  v1_nt[0] = n[0] * curr_vel.x + n[1] * curr_vel.y;
  v1_nt[1] = t[0] * curr_vel.x + t[1] * curr_vel.y;
  v2_nt[0] = n[0] * peg.velocity.x + n[1] * peg.velocity.y;
  v2_nt[1] = t[0] * peg.velocity.x + t[1] * peg.velocity.y;

  m1 = mass;
  m2 = peg.mass;
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
