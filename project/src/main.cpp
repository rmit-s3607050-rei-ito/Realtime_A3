// #include "player.h"
// #include "level.h"
#include "util.h"

Global global = {
  false,   // go
  false,   // wireframe

  0.0,     // elapsedTime
  0.0,     // startTime

  -0.5,    // bounce
  0.1,     // minVelocity

  8, // balls
  0, // score
};

Player player = {
  { 0.0, 0.0 },          // initPos
  { 0.0, 0.0 },          // currPos
  { 0.0, 0.0 },          // initVel
  { 0.0, 0.0 },          // currVel

  0.0,                   // rotation
  -180.0,                // minRotation
  0.0,                   // maxRotation
  1.0,                   // rotationInc
  5.0,                   // guideSize
  16.0,                  // guideSegments

  0.05,                  // radius
  0.0,                   // mass
  0.0,                   // elasticity

  0,                     // quadric
  10,                    // slices
  3,                     // loops
  { 0.5, 0.5, 1.0 },     // size
  { 0.0, 0.0, 0.0, 0.0 } // color
};

//Obstacle peg {
//  { 0.0, 0.0 }, // pos
//  { 1.0, 1.0 }, // vel
//
//  0.05, // radius
//  0.0, // mass
//  0.0, // elasticity
//
//  // Rendering params
//  0, // quadric
//  10, // slices
//  3, // loops
//  { 0.7, 0.7, 0.7 }, // size
//  { 0.0, 0.0, 1.0, 0.0 }, // color
//
//  false, // hit
//  false, // clear
//};

Obstacle pegs[20];

Launcher launcher;
Catcher catcher;

// ##### Misc functions #####
float degreesToRadians(float degrees) {
  float radians = degrees * (M_PI / 180);
  return radians;
}

static float getDeltaTime(void) {
  static float t1 = -1.0;

  if (t1 == -1)
    t1 = glutGet(GLUT_ELAPSED_TIME);

  float t2 = glutGet(GLUT_ELAPSED_TIME);
  float dt = (t2 - t1) / milli;
  t1 = t2;
  return dt;
}

// float roundDownFloat(int value) {
//   const int decimalPlace = 10;  // Round to 1 decimal place (in the tens)
//   float result;
//   result = floorf(value * decimalPlace) / decimalPlace;
//
//   return result;
// }

// ##### Initialization #####
void initLauncher(void) {
  // Base parameters
  launcher.radius = 0.08;
  launcher.slices = 64;
  launcher.loops = 1;

  // Strength of cannon
  launcher.power = LAUNCHER_POWER;

  // Quadric for base
  launcher.quadric = gluNewQuadric();

  // Cannon parameters
  launcher.length = LAUNCHER_LENGTH;
  launcher.width = LAUNCHER_WIDTH;

  launcher.cTopL = (vec2) { -launcher.width, launcher.length };
  launcher.cTopR = (vec2) { launcher.width, launcher.length };
  launcher.cBotL = (vec2) { -launcher.width, -launcher.length };
  launcher.cBotR = (vec2) { launcher.width, -launcher.length };
}

void initCatcher(void) {
  // Set default speed, moving leftwards
  catcher.velocity = CATCHER_SPEED;
  // Set how tall catcher is and size scaling
  catcher.height = CATCHER_HEIGHT;
  catcher.size = (vec3) { 1.0, 1.0, 1.0 };

  // Set positions for catcher left and right components
  // 1. Left section
  catcher.leftTL = (vec2) { -CATCHER_BUMPER_END, catcher.height };
  catcher.leftTR = (vec2) { -CATCHER_BUMPER_START, catcher.height };
  catcher.leftBL = (vec2) { -CATCHER_BUMPER_END, 0.0 };
  catcher.leftBR = (vec2) { -CATCHER_BUMPER_START, 0.0 };
  // 2. Right section
  catcher.rightTL = (vec2) { CATCHER_BUMPER_START, catcher.height };
  catcher.rightTR = (vec2) { CATCHER_BUMPER_END, catcher.height };
  catcher.rightBL = (vec2) { CATCHER_BUMPER_START, 0.0 };
  catcher.rightBR = (vec2) { CATCHER_BUMPER_END, 0.0 };

  // Set catcher to be at bottom of the level
  catcher.position.y = bottom;
  catcher.collisionY = catcher.position.y + catcher.height;
  // Set colors for the catcher
  catcher.sideColor = yellow;
  catcher.mainColor = brown;
}

void initPlayer(void) {
  // Player color
  player.color = white;

  // Positioning
  player.initPos = (vec2) { 0.0, 0.85 };
  player.currPos = player.initPos;

  // Initialize starting rotation
  player.rotation = STARTING_ROTATION;

  // Initialize quadric for rendering
  player.quadric = gluNewQuadric();
}

void initObstacle(Obstacle *peg) {
  peg->vel = { 1.0, 1.0 };

  peg->radius = 0.05;
  peg->mass = 0.0;
  peg->elasticity = 0.0;

  // Rendering params
  // Initialize quadric for rendering
  peg->quadric = gluNewQuadric();
  peg->slices = 10;
  peg->loops = 3;

  peg->size = { 0.7, 0.7, 0.7 };
  peg->color = { 0.0, 0.0, 1.0, 0.0 };

  peg->hit = false;
  peg->clear = false;
}

void initObstacles(void) {
  for (int i = 0; i < 10; i++) {
    initObstacle(&pegs[i]);
  }

  float range = 0.0;

  pegs[0].pos = { -0.85, 0.4 };
  pegs[1].pos = { -0.51, 0.4 };
  pegs[2].pos = { -0.17, 0.4 };
  pegs[3].pos = { 0.17, 0.4 };
  pegs[4].pos = { 0.51, 0.4 };
  pegs[5].pos = { -0.68, -0.4 };
  pegs[6].pos = { -0.34, -0.4 };
  pegs[7].pos = { 0.0, -0.4 };
  pegs[8].pos = { 0.34, -0.4 };
  pegs[9].pos = { 0.68, -0.4 };
}

void init(void) {
  initLauncher();
  initCatcher();
  initObstacles();
  initPlayer();
}

// ##### Drawing and display #####
void setRenderMode(void) {
  if(global.wireframe) {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_NORMALIZE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void setColoringMethod(color4f color) {
  if(global.wireframe)
    glColor4fv(&color.r);
  else
    glMaterialfv(GL_FRONT, GL_DIFFUSE, &color.r);
}

void drawLineStrip(vec2 start, vec2 end, color4f color) {
  glPushMatrix();
    setColoringMethod(color);
    glBegin(GL_LINE_STRIP);
      glVertex2f(start.x, start.y);
      glVertex2f(end.x, end.y);
    glEnd();
  glPopMatrix();
}

void drawSquare(vec2 topL, vec2 topR, vec2 botR, vec2 botL, color4f color) {
  glPushMatrix();
    setColoringMethod(color);
    glBegin(GL_POLYGON);
      glVertex2f(topL.x, topL.y);
      glVertex2f(topR.x, topR.y);
      glVertex2f(botR.x, botR.y);
      glVertex2f(botL.x, botL.y);
    glEnd();
  glPopMatrix();
}

void drawLauncher(void) {
  // Drawing player
  drawPlayer();

  glPushMatrix();
    glTranslatef(0.0, player.initPos.y, 0.0); // Move to start of screen

    // Drawing guide
    glPushMatrix();
      drawGuide();
    glPopMatrix();

    // Drawing launcher
    glPushMatrix();
      glRotatef(-STARTING_ROTATION, 0.0, 0.0, 1.0); // Cannon points downwards

      // Drawing cannon
      glPushMatrix();
        glRotatef(player.rotation, 0.0, 0.0, 1.0);  // Rotate with player rotation
        glTranslatef(0.0, - (LAUNCHER_LENGTH + LAUNCHER_LENGTH), 0.0);
        drawSquare(launcher.cTopL, launcher.cTopR, launcher.cBotR, launcher.cBotL, grey);
      glPopMatrix();

      // Drawing base
      glPushMatrix();
        setColoringMethod(darkGrey);
        gluDisk(launcher.quadric, 0.0, launcher.radius, launcher.slices, launcher.loops);
      glPopMatrix();
    glPopMatrix();

  glPopMatrix();
}

void drawCatcher(void) {
  // Draws entire catcher part by part
  glPushMatrix();
    glTranslatef(catcher.position.x, catcher.position.y, 0.0);
    glScalef(catcher.size.x, catcher.size.y, catcher.size.z);
    // 1. Left side of catcher, collision detected
    drawSquare(catcher.leftTL, catcher.leftTR, catcher.leftBR, catcher.leftBL,
               catcher.sideColor);
    // 3. Right side of catcher, collision detected
    drawSquare(catcher.rightTL, catcher.rightTR, catcher.rightBR, catcher.rightBL,
               catcher.sideColor);
    // 2. Middle, enable falling through
    drawSquare(catcher.leftTR, catcher.rightTL, catcher.rightBL, catcher.leftBR,
               catcher.mainColor);
  glPopMatrix();
}

void drawLevel(void) {
  /* 1. Draw 3 lines forming top and sides of game level, can collide with these
   * a. Line from Top Left -> Top Right */
  drawLineStrip(topLeft, topRight, white);
  // b. Line from Top Right -> Bottom Right
  drawLineStrip(topRight, botRight, white);
  // c. Line from Top Left -> Bottom Left
  drawLineStrip(topLeft, botLeft, white);

  // 2. Draw the catcher
  drawCatcher();

  // 3. Draw the launcher at the top
  drawLauncher();
}

void drawGuide(void) {
  if (!global.go && global.balls > 0) {
    vec2 pos, end;
    pos = (vec2) { 0.0, 0.0 };
    end.x = cos(degreesToRadians(player.rotation));
    end.y = sin(degreesToRadians(player.rotation));

    // Guide drawn as a parabola
    float tof = (end.y * 2.0) / -gravity;
    float stepSize = tof / player.guideSegments;
    float t;

    glPushMatrix();
      setColoringMethod(red);
      glBegin(GL_LINE_STRIP);
        for (float i = 0; i <= player.guideSegments; i++) {
          t = i * stepSize;
          pos.x = end.x * t;
          pos.y = (end.y * t) + (0.5 * gravity * t * t);

          // Invert both x and y
          pos.x = -pos.x;
          pos.y = -pos.y;

          // Scale guide size
          pos.x *= player.guideSize;
          pos.y *= player.guideSize;

          glVertex3f(pos.x, pos.y, 0.0);
        }
      glEnd();
    glPopMatrix();

    // Line version
    // glScalef(0.5, 0.5, 1.0);
    // drawLineStrip(pos, end, red);
  }
}

void drawPlayer(void) {
  /* Using disk drawing method in tutorial 9
   * Draw ball only when player has enough lives left */
  if (global.balls > 0) {
    glPushMatrix();
      setColoringMethod(player.color);
      glTranslatef(player.currPos.x, player.currPos.y, 0.0);
      glRotatef(player.rotation, 0.0, 0.0, 1.0);
      glScalef(player.size.x, player.size.y, player.size.z);
      gluDisk(player.quadric, 0.0, player.radius, player.slices, player.loops);
    glPopMatrix();
  }
}

void drawObstacles(void) {
  // Using disk drawing method in tutorial 9
  for (int i = 0; i < 10; i++) {
    if (!pegs[i].clear) {
      glPushMatrix();
        if (pegs[i].hit)
          pegs[i].color = { 0.0, 1.0, 1.0 };
        setColoringMethod(pegs[i].color);
        glTranslatef(pegs[i].pos.x, pegs[i].pos.y, 0.0);
        glScalef(pegs[i].size.x, pegs[i].size.y, pegs[i].size.z);
        gluDisk(pegs[i].quadric, 0.0, pegs[i].radius, pegs[i].slices, pegs[i].loops);
      glPopMatrix();
    }
  }
}

// ##### Game Logic implementation ######
void resetPlayer(void) {
  // Range for catcher, if land between these values ball is not lost
  float catcherStart = catcher.position.x + catcher.leftTR.x;
  float catcherEnd = catcher.position.x + catcher.rightTL.x;

  // Reduce life count by 1 if ball didnt end up in the catcher
  if (player.currPos.x < catcherStart || player.currPos.x > catcherEnd)
    global.balls--;

  // Reset player position to start of level and reset velocity
  player.currPos = player.initPos;
  player.currVel = player.initVel;

  // Allow player to launch again
  global.go = !global.go;

  for (int i = 0; i < 10; i++) {
    if (pegs[i].hit) {
      pegs[i].clear = true;
      global.score += 5;
    }
  }
  // printf("Position: %0.2f,%0.2f\n", player.currPos.x, player.currPos.y);
}

// ##### Movement and Collision detection #####
void moveCatcher(float dt) {
  float leftWall = left - catcher.leftTL.x;
  float rightWall = right - catcher.rightTR.x;

  // Move catcher at constant rate
  catcher.position.x += dt * catcher.velocity;

  // When catcher collide with side of wall, flip direction
  if (catcher.position.x <= leftWall || catcher.position.x >= rightWall)
    catcher.velocity *= -1;
}

void integrate(float dt) {
  // Uses analytical approach to movement calculations
  player.currPos.x += dt * player.currVel.x + (0.5 * gravity * dt * dt);
  player.currPos.y += dt * player.currVel.y + (0.5 * gravity * dt * dt);

  // Factor in gravity in ball movement
  player.currVel.y += gravity * dt;

  // Reset when fall out of level field
  if(player.currPos.y < bottom)
    resetPlayer();
}

void bruteForceWallCollide(float leftCollide, float rightCollide, float topCollide) {
  // Left Wall
  if (leftCollide <= left && player.currVel.x < global.minVelocity) {
    // printf("COLLIDED LEFT\n");
    player.currVel.x *= global.bounce;
  }
  // Right Wall
  else if (rightCollide >= right && player.currVel.x > global.minVelocity) {
    // printf("COLLIDED RIGHT\n");
    player.currVel.x *= global.bounce;
  }
  // Top Wall - Not part of else if as can collide with both top + left or right
  if (topCollide >= top && player.currVel.y > global.minVelocity) {
    // printf("COLLIDED TOP\n");
    player.currVel.y *= global.bounce;
  }
}

void bruteForceCatcherCollide(float xPos, float bottomCollide) {
  bool collided = false;

  float leftBumperStart = catcher.position.x + catcher.leftTL.x;
  float leftBumperEnd = catcher.position.x + catcher.leftTR.x;
  float rightBumperStart = catcher.position.x + catcher.rightTL.x;
  float rightBumperEnd = catcher.position.x + catcher.rightTR.x;

  // Check whether ball reached bottom where catcher is
  if (bottomCollide <= catcher.collisionY) {
    // Check whether left or right bumper has been collided with
    if (xPos > leftBumperStart && xPos < leftBumperEnd)
      collided = true;
    else if (xPos > rightBumperStart && xPos < rightBumperEnd)
      collided = true;

    if (collided) {
      // printf("COLLIDED WITH CATCHER\n");
      player.currVel.y *= global.bounce;
    }
  }
}

void bruteForceCollision() {
  // Radius of player, with scaling applied
  float playerRadius = (player.radius * player.size.x);

  // Values to compare for collision with ball
  float leftCollide = player.currPos.x - playerRadius;
  float rightCollide = player.currPos.x + playerRadius;
  float topCollide = player.currPos.y + playerRadius;
  float bottomCollide = player.currPos.y - playerRadius;

  // 2. Collisions against level wall
  bruteForceWallCollide(leftCollide, rightCollide, topCollide);

  // 3. Collision with sides of catcher
  bruteForceCatcherCollide(player.currPos.x, bottomCollide);

  // 3. Collisions against pegs
  // double radiusSum, radiusSumSqr, dissMagSqr;
  // vec2 diss;
  //
  // for (int i = 0; i < 10; i++) {
  //   if (!pegs[i].clear) {
  //     float pegRadius = (pegs[i].radius * pegs[i].size.x);
  //     radiusSum = playerRadius + pegRadius;
  //     radiusSumSqr = radiusSum*radiusSum;
  //     diss.x = pegs[i].pos.x - player.currPos.x;
  //     diss.y = pegs[i].pos.y - player.currPos.y;
  //     dissMagSqr = diss.x*diss.x + diss.y*diss.y;
  //     if (dissMagSqr <= radiusSumSqr) {
  //       if (!pegs[i].hit)
  //         global.score++;
  //
  //       pegs[i].hit = true;
  //
  //       // change rebound formula
  //       player.currVel.x *= global.bounce;
  //       player.currVel.y *= global.bounce;
  //     }
  //   }
  // }
}

// On screen display
void displayOSD() {
  char buffer[30];
  char *bufp;
  int w, h;

  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  /* Set up orthographic coordinate system to match the window,
     i.e. (0,0)-(w,h) */
  w = glutGet(GLUT_WINDOW_WIDTH);
  h = glutGet(GLUT_WINDOW_HEIGHT);
  glOrtho(0.0, w, 0.0, h, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glColor3f(1.0, 1.0, 0.0);
  glRasterPos2i((w/2)-27.5, h-22);
  snprintf(buffer, sizeof buffer, "PEGGLE");
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
  // Frame rate
  glRasterPos2i(10, h-22);
  snprintf(buffer, sizeof buffer, "Balls: %2d", global.balls);
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
  // Frame time
  glRasterPos2i(w-90, h-22);
  snprintf(buffer, sizeof buffer, "Score: %2d", global.score);
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);

  glPopMatrix();  /* Pop modelview */
  glMatrixMode(GL_PROJECTION);

  glPopMatrix();  /* Pop projection */
  glMatrixMode(GL_MODELVIEW);

  glPopAttrib();
}

// ##### Main functions #####
void update(void) {
  float dt = getDeltaTime();

  // Constantly move catcher
  moveCatcher(dt);

  // Move player and check for collisions only when launching
  if (global.go) {
    integrate(dt);
    bruteForceCollision();
  }

  glutPostRedisplay();
}

void display(void) {
  int err;

  glLoadIdentity(); // Set initial matrix
  glPushMatrix();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Apply render mode
  setRenderMode();

  // Draw level objects
  drawLevel();
  // drawPlayer();
  // drawObstacles();

  glPopMatrix();

  displayOSD();
  glutSwapBuffers();

  if ((err = glGetError()) != GL_NO_ERROR)
    printf("display: %s\n", gluErrorString(err));
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0, 1.0, -1.0, 1.0, -1, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    // Rotate launch position
    case 'a': // left
      if (!global.go && player.rotation > player.minRotation) {
        player.rotation -= player.rotationInc;
      }
      break;
    case 'd': // right
      if (!global.go && player.rotation < player.maxRotation) {
        player.rotation += player.rotationInc;
      }
      break;

    // Toggle wireframe/filled mode
    case '1':
      global.wireframe = !global.wireframe;
      break;

    case ' ': // 'space' = launch ball
      if(!global.go && global.balls > 0) {
        // Set velocity of x and y depending on direction rotated to
        player.currVel.x = cos(degreesToRadians(player.rotation)) * launcher.power;
        player.currVel.y = sin(degreesToRadians(player.rotation)) * launcher.power;
        // Get initial start time, enable resetting
        global.startTime = glutGet(GLUT_ELAPSED_TIME) / (float)milli;
        global.go = !global.go;
      }
      break;

    case 'q':
      exit(EXIT_SUCCESS);
      break;

    case 27:
      exit(0);
      break;

    default:
      break;
  }
  glutPostRedisplay();
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WINDOW_X, WINDOW_Y);
  glutInitWindowPosition(WINDOW_POS_X, WINDOW_POS_Y);
  glutCreateWindow(argv[0]);

  init();

  glutDisplayFunc(display);
  glutIdleFunc(update);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();

  return 0;
}
