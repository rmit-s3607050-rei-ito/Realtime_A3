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

Obstacle peg = {
  { 0.0, 0.0 }, // pos
  { 1.0, 1.0 }, // vel

  0.05, // radius
  0.0, // mass
  0.0, // elasticity

  // Rendering params
  0,
  10, // slices
  3, // loops
  { 0.7, 0.7, 0.7 }, // size
  { 0.0, 0.0, 1.0, 0.0 }, // color

  false, // hit
};

Catcher catcher = {
  { 0.0, 0.0 },          // position
  { 0.0, 0.0 } ,         // leftStart
  { 0.0, 0.0 },          // leftEnd
  { 0.0, 0.0 },          // rightStart
  { 0.0, 0.0 },          // rightEnd

  1.0,                   // speed
  true,                  // moveLeft

  0.025,                  // height
  { 1.0, 1.0, 1.0 },     // size
  { 0.0, 0.0, 0.0, 0.0 } // color
};

// ##### Misc functions #####
float degreesToRadians(float degrees) {
  float radians = degrees * (M_PI / 180);
  return radians;
}

// float roundDownFloat(int value) {
//   const int decimalPlace = 10;  // Round to 1 decimal place (in the tens)
//   float result;
//   result = floorf(value * decimalPlace) / decimalPlace;
//
//   return result;
// }

// ##### Initialization #####
void initCatcher(void) {
  // Set positions for all catcher components
  catcher.leftStart.x = -0.25;
  catcher.leftEnd = (vec2) { -0.15, catcher.height };
  catcher.rightStart = (vec2) { 0.15, catcher.height };
  catcher.rightEnd.x = 0.25;
  // Set catcher to be at bottom of the level
  catcher.position.y = bottom - catcher.height;
  // Set color for the catcher
  catcher.color = yellow;
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

void initObstacles(void) {
  // Initialize quadric for rendering
  peg.quadric = gluNewQuadric();
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

void drawPlayer(void) {
  // Using disk drawing method in tutorial 9
  glPushMatrix();
    setColoringMethod(player.color);
    glTranslatef(player.currPos.x, player.currPos.y, 0.0);
    glRotatef(player.rotation, 0.0, 0.0, 1.0);
    glScalef(player.size.x, player.size.y, player.size.z);
    gluDisk(player.quadric, 0.0, player.radius, player.slices, player.loops);
  glPopMatrix();
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

void drawObstacles(void) {
  // Using disk drawing method in tutorial 9
  glPushMatrix();
    if (peg.hit)
      peg.color = { 1.0, 0.0, 1.0 };
    setColoringMethod(peg.color);
    glTranslatef(peg.pos.x, peg.pos.y, 0.0);
    glScalef(peg.size.x, peg.size.y, peg.size.z);
    gluDisk(peg.quadric, 0.0, peg.radius, peg.slices, peg.loops);
  glPopMatrix();
}

// ##### Game Logic implemtation ######
void resetPlayer() {
  // Reduce life count by 1

  // Reset player position to start of level and reset velocity
  player.currPos = player.initPos;
  player.currVel = player.initVel;

  // Allow player to launch again
  global.go = !global.go;
}

// ##### Movement and Collision detection #####
<<<<<<< HEAD
void moveCatcher(float dt) {
  if (catcher.moveLeft)
    catcher.position.x -= dt * catcher.speed;
  else
    catcher.position.x += dt * catcher.speed;
}

=======
>>>>>>> 653e44d47d8fd29d1d8f7668547bf9d08b03da57
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

void bruteForceCollision() {
  float playerRadius = (player.radius * player.size.x);
  float pegRadius = (peg.radius * peg.size.x);

  // Values to compare for collision with walls, scaled with size for precision
  float leftCollide = player.currPos.x - playerRadius;
  float rightCollide = player.currPos.x + playerRadius;
  float topCollide = player.currPos.y + playerRadius;

  // 1. Collisions between player ball and pegs

  // 2. Collisions against level wall
  if (leftCollide <= left && player.currVel.x < global.minVelocity) {
    printf("COLLIDED LEFT\n");
    player.currVel.x *= global.bounce;
  }

  if (rightCollide >= right && player.currVel.x > global.minVelocity) {
    printf("COLLIDED RIGHT\n");
    player.currVel.x *= global.bounce;
  }

  if (topCollide >= top && player.currVel.y > global.minVelocity) {
    printf("COLLIDED TOP\n");
    player.currVel.y *= global.bounce;
  }

  // double radiusSum, radiusSumSqr, dissMagSqr;
  // vec2 diss;
  //
  // radiusSum = playerRadius + pegRadius;
  // radiusSumSqr = radiusSum*radiusSum;
  // diss.x = peg.pos.x - player.currPos.x;
  // diss.y = peg.pos.y - player.currPos.y;
  // dissMagSqr = diss.x*diss.x + diss.y*diss.y;
  // if (dissMagSqr <= radiusSumSqr) {
  //   peg.hit = true;
  //   player.currVel.x *= global.bounce;
  //   player.currVel.y *= global.bounce;
  // }
}

// ##### Main functions #####
void update(void) {
  static float lastT = -1.0;
  float t, dt;  // time, delta time

  t = glutGet(GLUT_ELAPSED_TIME) / (float)milli - global.startTime;
  dt = t - lastT;

  if (!global.go) {
    lastT = -1.0;
    return;
  }

  if (lastT < 0) {
    lastT = t;
    return;
  }

  // Move player
  integrate(dt);
  // Check for collisions
  bruteForceCollision();

  lastT = t;

  glutPostRedisplay();
}

void display(void) {
  int err;

  glLoadIdentity(); // Set initial matrix
  glPushMatrix();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Apply render mode
  setRenderMode();

  // Draw guide
  drawGuide();

  // Draw level objects
  drawLevel();
  drawPlayer();
  // drawObstacles();

  glPopMatrix();
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
      if(!global.go) {
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
