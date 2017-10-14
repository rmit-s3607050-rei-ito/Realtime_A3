
#include "player.h"

void initPlayer(Player *player) {
  // Positioning
  player->initPos = { 0.0, PLAYER_START_POS_Y };
  player->currPos = player->initPos;
  player->initVel = { 0.0, 0.0 };
  player->currVel = player->initVel;
  player->size = { PLAYER_SIZE, PLAYER_SIZE, 1.0 };

  // Rotations
  player->rotation = STARTING_ROTATION;
  player->minRotation = MIN_ROTATION;
  player->maxRotation = MAX_ROTATION;
  player->rotationInc = ROTATION_INCREMENT;

  // Trajectory guide
  player->guideSize = GUIDE_SIZE;
  player->guideSegments = GUIDE_SEGMENTS;

  // Launch strength
  player->power = LAUNCH_POWER;

  // Collision
  player->collisionRadius = player->radius * player->size.x;

  // Drawing + internal params
  player->segments = PLAYER_SEGMENTS;
  player->radius = PLAYER_RADIUS;
  player->mass = PLAYER_MASS;
  player->elasticity = PLAYER_ELASTICITY;

  // Color
  player->color = white;
}

void drawGuide(Player *player) {
  glm::vec2 pos = { 0.0, 0.0 };
  glm::vec2 vel;
  vel.x = cos(degreesToRadians(player->rotation)) * player->power;
  vel.y = sin(degreesToRadians(player->rotation)) * player->power;

  // Guide drawn as a parabola
  float tof = (vel.y * 2.0) / gravity;
  float stepSize = tof / player->guideSegments;
  float t;

  glPushMatrix();
    glTranslatef(0.0, PLAYER_START_POS_Y, 0.0); // Move to top of level
    setColoringMethod(red);
    glBegin(GL_LINE_STRIP);
      for (float i = 0; i <= player->guideSegments; i++) {
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

void drawPlayer(Player *player) {
  glPushMatrix();
    setColoringMethod(player->color);
    glTranslatef(player->currPos.x, player->currPos.y, 0.0);
    glRotatef(player->rotation, 0.0, 0.0, 1.0);
    glScalef(player->size.x, player->size.y, player->size.z);
    drawCircle(player->segments, player->radius);
  glPopMatrix();
}

void setLaunch(Player *player) {
  player->currVel.x = cos(degreesToRadians(player->rotation)) * player->power;
  player->currVel.y = sin(degreesToRadians(player->rotation)) * player->power;
}

void integrate(float dt, Player *player) {
  // Uses analytical approach to movement calculations
  player->currPos.x += dt * player->currVel.x;
  player->currPos.y += dt * player->currVel.y + (0.5 * gravity * dt * dt);

  // Factor in gravity in ball movement
  player->currVel.y += gravity * dt;
}
