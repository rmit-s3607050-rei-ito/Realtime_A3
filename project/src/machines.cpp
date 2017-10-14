
#include "machines.h"

// Initialization
void initLauncher(Launcher *launcher) {
  // Base parameters
  launcher->segments = LAUNCHER_SEGMENTS;
  launcher->radius = LAUNCHER_RADIUS;

  // Cannon parameters
  launcher->length = LAUNCHER_LENGTH;
  launcher->width = LAUNCHER_WIDTH;

  // Positioning to draw shapes
  launcher->cTopL = (vec2) { -launcher->width, launcher->length };
  launcher->cTopR = (vec2) { launcher->width, launcher->length };
  launcher->cBotL = (vec2) { -launcher->width, -launcher->length };
  launcher->cBotR = (vec2) { launcher->width, -launcher->length };

  // Coloring
  launcher->cannonColor = grey;
  launcher->baseColor = darkGrey;
}

void initCatcher(Catcher *catcher) {
  // Set default speed, moving leftwards
  catcher->velocity = CATCHER_SPEED;
  // Set how tall catcher is and size scaling
  catcher->height = CATCHER_HEIGHT;
  catcher->size = (vec3) { 1.0, 1.0, 1.0 };

  // Set positions for catcher left and right components
  // 1. Left section
  catcher->leftTL = (vec2) { -CATCHER_BUMPER_END, catcher->height };
  catcher->leftTR = (vec2) { -CATCHER_BUMPER_START, catcher->height };
  catcher->leftBL = (vec2) { -CATCHER_BUMPER_END, 0.0 };
  catcher->leftBR = (vec2) { -CATCHER_BUMPER_START, 0.0 };
  // 2. Right section
  catcher->rightTL = (vec2) { CATCHER_BUMPER_START, catcher->height };
  catcher->rightTR = (vec2) { CATCHER_BUMPER_END, catcher->height };
  catcher->rightBL = (vec2) { CATCHER_BUMPER_START, 0.0 };
  catcher->rightBR = (vec2) { CATCHER_BUMPER_END, 0.0 };

  // Set catcher to be at bottom of the level
  catcher->position.y = BOTTOM;
  catcher->collisionY = catcher->position.y + catcher->height;

  // Set colors for the catcher
  catcher->sideColor = yellow;
  catcher->mainColor = brown;
}

// Drawing
void drawLauncher(Launcher *launcher, Player *player) {
  glPushMatrix();
    glTranslatef(0.0, PLAYER_START_POS_Y, 0.0); // Move to top of level

    // Drawing launcher
    glPushMatrix();
      glRotatef(-STARTING_ROTATION, 0.0, 0.0, 1.0); // Cannon points downwards

      // Drawing cannon
      glPushMatrix();
        glRotatef(player->rotation, 0.0, 0.0, 1.0);  // Rotate with player rotation
        glTranslatef(0.0, - (LAUNCHER_LENGTH + LAUNCHER_LENGTH), 0.0);
        drawSquare(launcher->cTopL, launcher->cTopR, launcher->cBotR, launcher->cBotL,
                   launcher->cannonColor);
      glPopMatrix();

      // Drawing base
      glPushMatrix();
        setColoringMethod(launcher->baseColor);
        drawCircle(launcher->segments, launcher->radius);
      glPopMatrix();
    glPopMatrix();

  glPopMatrix();
}

void drawCatcher(Catcher *catcher) {
  // Draws entire catcher part by part
  glPushMatrix();
    glTranslatef(catcher->position.x, catcher->position.y, 0.0);
    glScalef(catcher->size.x, catcher->size.y, catcher->size.z);
    // 1. Left side of catcher, collision detected
    drawSquare(catcher->leftTL, catcher->leftTR, catcher->leftBR, catcher->leftBL,
               catcher->sideColor);
    // 3. Right side of catcher, collision detected
    drawSquare(catcher->rightTL, catcher->rightTR, catcher->rightBR, catcher->rightBL,
               catcher->sideColor);
    // 2. Middle, enable falling through
    drawSquare(catcher->leftTR, catcher->rightTL, catcher->rightBL, catcher->leftBR,
               catcher->mainColor);
  glPopMatrix();
}

// Movement and Collision
void moveCatcher(Catcher *catcher, float dt) {
  float leftWall = LEFT - catcher->leftTL.x;
  float rightWall = RIGHT - catcher->rightTR.x;

  // Move catcher at constant rate
  catcher->position.x += dt * catcher->velocity;

  // When catcher collide with side of wall, flip direction
  if (catcher->position.x <= leftWall) {
    catcher->position.x += 2.0 * (leftWall - catcher->position.x);
    catcher->velocity *= CATCHER_REBOUND;
  }
  else if (catcher->position.x >= rightWall) {
    catcher->position.x += 2.0 * (rightWall - catcher->position.x);
    catcher->velocity *= CATCHER_REBOUND;
  }
}

void catcherCollide(Catcher *catcher, Player *player, float bottomCollide) {
  float leftBumperStart = catcher->position.x + catcher->leftTL.x;
  float leftBumperEnd = catcher->position.x + catcher->leftTR.x;
  float rightBumperStart = catcher->position.x + catcher->rightTL.x;
  float rightBumperEnd = catcher->position.x + catcher->rightTR.x;

  float xPos = player->currPos.x;

  // Check whether ball reached bottom where catcher is
  if (bottomCollide <= catcher->collisionY) {
    // Check whether left or right bumper has been collided with
    if (xPos > leftBumperStart && xPos < leftBumperEnd) {
      player->currPos.y += 2.0 * (BOTTOM + catcher->height - bottomCollide);
      player->currVel.y *= -1.0;
    } else if (xPos > rightBumperStart && xPos < rightBumperEnd) {
      player->currPos.y += 2.0 * (BOTTOM + catcher->height - bottomCollide);
      player->currVel.y *= -1.0;
    }
  }
}
