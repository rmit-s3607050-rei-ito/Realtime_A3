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

  2.0,                   // power

  270.0,                 // rotation
  1.0,                   // rotationInc
  180.0,                 // minRotation
  360.0,                 // maxRotation
  0.5,                   // guideSize

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

float roundDownFloat(int value) {
  const int decimalPlace = 10;  // Round to 1 decimal place (in the tens)
  float result;
  result = floorf(value * decimalPlace) / decimalPlace;

  return result;
}

// ##### Initialization #####
void initCatcher(void) {
  // Set default speed, moving leftwards
  catcher.velocity = CATCHER_SPEED;
  // Set how tall catcher is and size scaling
  catcher.height = CATCHER_HEIGHT;
  catcher.size = (vec3) { 1.0, 1.0, 1.0 };

  // Set positions for all catcher components
  catcher.leftStart.x = -0.3;
  // catcher.leftStart.x = -0.5;
  catcher.leftEnd = (vec2) { -0.18, catcher.height };
  catcher.rightStart = (vec2) { 0.18, catcher.height };
  catcher.rightEnd.x = 0.3;
  // catcher.rightEnd.x = 0.5;
  // Set catcher to be at bottom of the level
  catcher.position.y = bottom - catcher.height;
  // Set color for the catcher
  catcher.color = brown;
}

void initPlayer(void) {
  // Default color is greyish
  player.color = grey;

  // Positioning
  player.initPos = (vec2) { 0.0, 0.85 };
  player.currPos = player.initPos;

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
    // Draw the line
    glBegin(GL_LINE_STRIP);
      glVertex2f(start.x, start.y);
      glVertex2f(end.x, end.y);
    glEnd();
  glPopMatrix();
}

void drawCatcher(void) {
  glPushMatrix();
    setColoringMethod(catcher.color);
    glTranslatef(catcher.position.x, catcher.position.y, 0.0);
    glScalef(catcher.size.x, catcher.size.y, catcher.size.z);
    // Drawing entire catcher part by part
    glBegin(GL_POLYGON);
      // Left side of catcher, collision detected
      glVertex2f(catcher.leftStart.x, catcher.leftStart.y);
      glVertex2f(catcher.leftEnd.x, catcher.leftEnd.y);
      // Middle, enable falling through
      glVertex2f(catcher.leftEnd.x, catcher.leftEnd.y);
      glVertex2f(catcher.rightStart.x, catcher.rightStart.y);
      // Right side of catcher, collision detected
      glVertex2f(catcher.rightStart.x, catcher.rightStart.y);
      glVertex2f(catcher.rightEnd.x, catcher.rightEnd.y);
      // Bottom of catcher
      glVertex2f(catcher.rightEnd.x, catcher.rightEnd.y);
      glVertex2f(catcher.leftStart.x, catcher.leftStart.y);
    glEnd();
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
}

void drawGuide(void) {
  // Only show guide pre launch
  // if (!global.go) {
    // Calculating end point, * size scaling
    vec2 end;
    end.x = cos(degreesToRadians(player.rotation));
    end.y = sin(degreesToRadians(player.rotation));

    // Scale size
    end.x *= player.guideSize;
    end.y *= player.guideSize;

    // Attach guide to initialPosition
    end.x += player.initPos.x;
    end.y += player.initPos.y;

    drawLineStrip(player.initPos, end, red);
  // }
}

void drawPlayer(void) {
  /* Using disk drawing method in tutorial 9
   * Draw ball only when player has enough lives left */
  if (global.balls > 0) {
    drawGuide();
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

// ##### Game Logic implemtation ######
void resetPlayer(void) {
  // Range for catcher, if land between these values ball is not lost
  float catcherStart = catcher.position.x + catcher.leftEnd.x;
  float catcherEnd = catcher.position.x + catcher.rightStart.x;

  // Reduce life count by 1 if landed of bounds, or increase by 1 if in catcher
  if (player.currPos.x < catcherStart || player.currPos.x > catcherEnd) {
    global.balls--;
    printf("Ball lost\n");
  } else {
    printf("Balls unchanged\n");
  }
  printf("Balls left: %d\n", global.balls);

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
  float leftWall = left - catcher.leftEnd.x;
  float rightWall = right - catcher.rightStart.x;

  // Move catcher at constant rate
  catcher.position.x += dt * catcher.velocity;

  // When catcher collide with side of wall, flip direction
  if (catcher.position.x <= leftWall || catcher.position.x >= rightWall)
    catcher.velocity *= -1;
}

void integrate(float dt) {
  // Uses analytical approach to movement calculations
  player.currPos.x += dt * player.currVel.x + 0.5 * gravity * dt * dt;
  player.currPos.y += dt * player.currVel.y + 0.5 * gravity * dt * dt;

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

void bruteForceCatcherCollide(float leftCollide, float rightCollide, float bottomCollide) {
  bool collide = false;
  float bottomY, topY;

  float rightBumperStart = catcher.position.x + catcher.rightStart.x;
  float rightBumperEnd = catcher.position.x + catcher.rightEnd.x;
  float leftBumperStart = catcher.position.x + catcher.leftStart.x;
  float leftBumperEnd = catcher.position.x + catcher.leftEnd.x;

  // 1. Check side of collision:
  // 1a. Left side of ball (Right of catcher)
  if (leftCollide > rightBumperStart && leftCollide < rightBumperEnd) {
    collide = true;
    // printf("HIT RIGHT OF CATCHER\n");
  }
  // 1b. Right side of ball (Left of catcher)
  else if (rightCollide > leftBumperStart && rightCollide < leftBumperEnd) {
    collide = true;
    // printf("HIT LEFT OF CATCHER\n");
  }

  // 2. If collision in x, ensure collision only occurs when y pos reached
  if (collide) {
    // bottomY = bottom;
    // topY = bottomY + catcher.height;
    // // Bounce off only when point of collision reached
    // if (bottomCollide >= bottomY && bottomCollide <= topY) {
    //   player.currVel.y *= global.bounce;
    // }
    // bottomY = bottom;
    // topY = bottomY + catcher.height;
    // Bounce off only when point of collision reached
    if (bottomCollide <= bottom + catcher.height) {
      printf("COLLIDED WITH CATCHER\n");
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
  bruteForceCatcherCollide(leftCollide, rightCollide, bottomCollide);

  // 3. Collisions against pegs
  double radiusSum, radiusSumSqr, dissMagSqr;
  vec2 diss;

  for (int i = 0; i < 10; i++) {
    if (!pegs[i].clear) {
      float pegRadius = (pegs[i].radius * pegs[i].size.x);
      radiusSum = playerRadius + pegRadius;
      radiusSumSqr = radiusSum*radiusSum;
      diss.x = pegs[i].pos.x - player.currPos.x;
      diss.y = pegs[i].pos.y - player.currPos.y;
      dissMagSqr = diss.x*diss.x + diss.y*diss.y;
      if (dissMagSqr <= radiusSumSqr) {
        if (!pegs[i].hit)
          global.score++;

        pegs[i].hit = true;

        // change rebound formula
        player.currVel.x *= global.bounce;
        player.currVel.y *= global.bounce;
      }
    }
  }
}

// On screen display
void displayOSD()
{
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
    // Check for collisions
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
  drawPlayer();
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
        player.currVel.x = cos(degreesToRadians(player.rotation)) * player.power;
        player.currVel.y = sin(degreesToRadians(player.rotation)) * player.power;
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
