// #include "player.h"
// #include "level.h"
#include "util.h"

Global global = {
  false,   // go
  false,   // wireframe

  0.0,     // dt

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

  8.0,                   // segments
  0.05,                  // radius
  100.0,                   // mass
  1.0,                   // elasticity

  { 0.5, 0.5, 1.0 },     // size
  { 0.0, 0.0, 0.0, 0.0 } // color
};

Obstacle pegs[HEIGHT][WIDTH];
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

// ##### Initialization #####
void initLauncher(void) {
  // Base parameters
  launcher.segments = LAUNCHER_SEGMENTS;
  launcher.radius = LAUNCHER_RADIUS;

  // Strength of cannon
  launcher.power = LAUNCHER_POWER;

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

  player.initVel.x = cos(degreesToRadians(player.rotation));
  player.initVel.y = sin(degreesToRadians(player.rotation));

  // Initialize starting rotation
  player.rotation = STARTING_ROTATION;
}

void initObstacle(Obstacle *peg) {
  peg->vel = { 0.0, 0.0 };

  peg->radius = 0.05;
  peg->mass = 5.0;
  peg->elasticity = 1.0;

  // Rendering params
  peg->segments = 8.0;
  peg->size = { 0.7, 0.7, 0.7 };
  peg->color = { 0.0, 0.0, 1.0, 0.0 };

  peg->hit = false;
  peg->clear = false;
  peg->empty = false;
}

void initObstacles(void) {
  float leftLimit = left+0.1;
  float rightLimit = right;
  float bottomLimit = bottom+0.35;
  float topLimit = top;

  float xInterval = (fabs(leftLimit-rightLimit))/WIDTH;
  float xCurr = leftLimit;

  float yInterval = (fabs(bottomLimit-topLimit))/HEIGHT;
  float yCurr = bottomLimit;

  for(int row = 0; row < HEIGHT; row++) {
    for (int col = 0; col < WIDTH; col++) {
      if(((col + row) % 2) == 0) {
        initObstacle(&pegs[row][col]);
        pegs[row][col].pos = { xCurr, yCurr };
      } else
        pegs[row][col].empty = true;
      xCurr+=xInterval;
    }
    xCurr = leftLimit;
    yCurr+=yInterval;
  }
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

void drawCircle(float segments, float radius) {
  vec2 pos;
  float theta;

  glBegin(GL_POLYGON);
    for (int i = 0; i <= segments * 2.0; i++) {
      theta = i / segments * M_PI;
      pos.x = radius * cosf(theta);
      pos.y = radius * sinf(theta);

      glVertex3f(pos.x, pos.y, 0.0);
    }
  glEnd();
}

void drawLauncher(void) {
  // Drawing player
  drawPlayer();

  glPushMatrix();
    glTranslatef(0.0, player.initPos.y, 0.0); // Move to top of level

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
        drawCircle(launcher.segments, launcher.radius);
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
    vec2 start, vel;
    vel.x = cos(degreesToRadians(player.rotation)) * launcher.power;
    vel.y = sin(degreesToRadians(-player.rotation)) * launcher.power;

    // Guide drawn as a parabola
    float tof = (vel.y * 2.0) / -gravity;
    float stepSize = tof / player.guideSegments;
    float t;

    setColoringMethod(red);
    glBegin(GL_LINE_STRIP);
      for (float i = 0; i <= player.guideSegments; i++) {
        t = i * stepSize;
        start.x = t * vel.x;
        start.y = (vel.y * t) + (0.5 * gravity * t * t);
        vel.y += gravity * t;

        // Invert both x and y
        // pos.x = -pos.x;
        // pos.y = -pos.y;

        glVertex3f(start.x, start.y, 0.0);
      }
    glEnd();
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
      drawCircle(player.segments, player.radius);
    glPopMatrix();
  }
}

void drawObstacles(void) {
  // Using disk drawing method in tutorial 9
  for(int row = 0; row < HEIGHT; row++) {
    for (int col = 0; col < WIDTH; col++) {
      if (!pegs[row][col].clear && !pegs[row][col].empty) {
        glPushMatrix();
          if (pegs[row][col].hit)
            pegs[row][col].color = { 0.0, 1.0, 1.0 };
          setColoringMethod(pegs[row][col].color);
          glTranslatef(pegs[row][col].pos.x, pegs[row][col].pos.y, 0.0);
          glScalef(pegs[row][col].size.x, pegs[row][col].size.y, pegs[row][col].size.z);
          drawCircle(pegs[row][col].segments, pegs[row][col].radius);
        glPopMatrix();
      }
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

  // Remove all hit pegs and add to score per hit peg
  for(int row = 0; row < HEIGHT; row++) {
    for (int col = 0; col < WIDTH; col++) {
      if (!pegs[row][col].clear && !pegs[row][col].empty) {
        if (pegs[row][col].hit) {
          pegs[row][col].clear = true;
          global.score += 5;
        }
      }
    }
  }
}

// ##### Movement and Collision detection #####
void moveCatcher(float dt) {
  float leftWall = left - catcher.leftTL.x;
  float rightWall = right - catcher.rightTR.x;

  // Move catcher at constant rate
  catcher.position.x += dt * catcher.velocity;

  // When catcher collide with side of wall, flip direction
  if (catcher.position.x <= leftWall) {
    catcher.position.x += 2.0 * (leftWall - catcher.position.x);
    catcher.velocity *= -1.0;
  }
  else if (catcher.position.x >= rightWall) {
    catcher.position.x += 2.0 * (rightWall - catcher.position.x);
    catcher.velocity *= -1.0;
  }
}

void integrate(float dt) {
  // Uses analytical approach to movement calculations
  player.currPos.x += dt * player.currVel.x;
  player.currPos.y += dt * player.currVel.y + (0.5 * gravity * dt * dt);

  // Factor in gravity in ball movement
  player.currVel.y += gravity * dt;

  // Reset when fall out of level field
  if(player.currPos.y < bottom)
    resetPlayer();
}

void wallCollide(float leftCollide, float rightCollide, float topCollide) {
  // Based on tutorial 10 wall collision
  // Left wall hit
  if (leftCollide <= left) {
    player.currPos.x += 2.0 * (left - leftCollide);
    player.currVel.x *= -1.0;
  }
  else if (rightCollide >= right) {
  // Right wall hit
    player.currPos.x += 2.0 * (right - rightCollide);
    player.currVel.x *= -1.0;
  }
  // Top wall hit - Can collide with both top + left/right (corner)
  if (topCollide >= top) {
    player.currPos.y += 2.0 * (top - topCollide);
    player.currVel.y *= -1.0;
  }
}

void bruteForceCatcherCollide(float xPos, float bottomCollide) {
  float leftBumperStart = catcher.position.x + catcher.leftTL.x;
  float leftBumperEnd = catcher.position.x + catcher.leftTR.x;
  float rightBumperStart = catcher.position.x + catcher.rightTL.x;
  float rightBumperEnd = catcher.position.x + catcher.rightTR.x;

  // Check whether ball reached bottom where catcher is
  if (bottomCollide <= catcher.collisionY) {
    // Check whether left or right bumper has been collided with
    if (xPos > leftBumperStart && xPos < leftBumperEnd) {
      player.currPos.y += 2.0 * (bottom + catcher.height - bottomCollide);
      player.currVel.y *= -1.0;
    } else if (xPos > rightBumperStart && xPos < rightBumperEnd) {
      player.currPos.y += 2.0 * (bottom + catcher.height - bottomCollide);
      player.currVel.y *= -1.0;
    }
  }
}

void bruteForceCollision() {
  // Radius of player, with scaling applied
  float playerRadius = (player.radius * player.size.x);

  // Values to compare for collision with ball
  float playerLeft = player.currPos.x - playerRadius;
  float playerRight = player.currPos.x + playerRadius;
  float playerTop = player.currPos.y + playerRadius;
  float playerBottom = player.currPos.y - playerRadius;

  // 2. Collisions against level wall
  wallCollide(playerLeft, playerRight, playerTop);

  // 3. Collision with sides of catcher
  bruteForceCatcherCollide(player.currPos.x, playerBottom);

  // 3. Collisions against pegs
  double radiusSum, radiusSumSqr, dissMagSqr;
  vec2 diss;

  for(int row = 0; row < HEIGHT; row++) {
    for (int col = 0; col < WIDTH; col++) {
      if (!pegs[row][col].clear && !pegs[row][col].empty) {
        float pegRadius = (pegs[row][col].radius * pegs[row][col].size.x);

        radiusSum = playerRadius + pegRadius;
        radiusSumSqr = radiusSum * radiusSum;

        diss.x = pegs[row][col].pos.x - player.currPos.x;
        diss.y = pegs[row][col].pos.y - player.currPos.y;

        dissMagSqr = (diss.x * diss.x) + (diss.y * diss.y);
        if (dissMagSqr <= radiusSumSqr) {
          float n[2], t[2], n_mag;
          float v1_nt[2], v2_nt[2];
          float m1, m2, v1i, v2i, v1f, v2f;

          n[0] = pegs[row][col].pos.x - player.currPos.x;;
          n[1] = pegs[row][col].pos.y - player.currPos.y;;

          n_mag = sqrt(n[0] * n[0] + n[1] * n[1]);
          n[0] /= n_mag;
          n[1] /= n_mag;

          t[0] = -n[1];
          t[1] = n[0];

          v1_nt[0] = n[0] * player.currVel.x + n[1] * player.currVel.y;
          v1_nt[1] = t[0] * player.currVel.x + t[1] * player.currVel.y;
          v2_nt[0] = n[0] * 0 + n[1] * 0;
          v2_nt[1] = t[0] * 0 + t[1] * 0;

          m1 = player.mass;
          m2 = pegs[row][col].mass;
          v1i = v1_nt[0];
          v2i = v2_nt[0];
          v1f = (m1 - m2) / (m1 + m2) * v1i + 2.0 * m2 / (m1 + m2) * v2i;
          v2f = 2.0 * m1 / (m1 + m2) * v1i + (m2 - m1) / (m1 + m2) * v2i;

          v1_nt[0] = v1f;
          v2_nt[0] = v2f;

          player.currVel.x = (n[0] * v1_nt[0] + t[0] * v1_nt[1])
            - (n[0] * v2_nt[0] + t[0] * v2_nt[1]);
          player.currVel.y = (n[1] * v1_nt[0] + t[1] * v1_nt[1])
            - (n[1] * v2_nt[0] + t[1] * v2_nt[1]);

          // If peg not already hit, add to score
          if (!pegs[row][col].hit)
            global.score += 1;

          pegs[row][col].hit = true;
        }
      }
    }
  }
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
  snprintf(buffer, sizeof buffer, "Balls: %d", global.balls);
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
  // Frame time
  glRasterPos2i(w-90, h-22);
  snprintf(buffer, sizeof buffer, "Score: %d", global.score);
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
  // float dt = getDeltaTime();
  global.dt = getDeltaTime();

  // Constantly move catcher
  moveCatcher(global.dt);

  // Move player and check for collisions only when launching
  if (global.go) {
    integrate(global.dt);
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
  drawObstacles();

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
      if (!global.go && player.rotation > player.minRotation)
        player.rotation -= player.rotationInc;
      break;
    case 'd': // right
      if (!global.go && player.rotation < player.maxRotation)
        player.rotation += player.rotationInc;
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
