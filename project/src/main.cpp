// #include "player.h"
// #include "level.h"
#include "util.h"

Global global = {
  false,   // go
  false,   // wireframe

  0.0,     // elapsedTime
  0.0,     // startTime
};

Player player = {
  { 0.0, 0.0 },          // initPos
  { 0.0, 0.0 },          // currPos

  0.0,                   // rotation
  1.0,                   // rotationInc
  180.0,                 // minRotation
  360.0,                 // maxRotation
  0.5,                   // guideSize

  { 1.0, 1.0 },          // velocity
  0.05,                  // radius
  0.0,                   // mass
  0.0,                   // elasticity

  0,                     // quadric
  10,                    // slices
  3,                     // loops
  { 0.65, 0.65, 1.0 },   // size
  { 0.0, 0.0, 0.0, 0.0 } // color
};

Obstacle peg {
  { 0.0, 0.0 }, // pos

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

// ##### Misc functions #####
float degreesToRadians(float degrees) {
  float radians = degrees * (M_PI / 180);
  return radians;
}

// ##### Initialization #####
void initPlayer(void) {
  // Default color is greyish
  player.playerColor = grey;

  // Positioning
  player.initPos = (vec2) { 0.0, 0.87 };
  player.currPos = player.initPos;

  printf("Initialize with position: %f,%f\n", player.currPos.x, player.currPos.y);

  // Initialize quadric for rendering
  player.quadric = gluNewQuadric();
}

void initObstacles(void) {
  // Initialize quadric for rendering
  peg.quadric = gluNewQuadric();
}

void init(void) {
  initPlayer();
  initObstacles();
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

void drawLevelWindow(void) {
  /* Draw 3 lines forming top and sides of game level, can collide with these
   * 1. Line from Top Left -> Top Right */
  drawLineStrip(topLeft, topRight, white);
  // 2. Line from Top Right -> Bottom Right
  drawLineStrip(topRight, botRight, white);
  // 3. Line from Top Left -> Bottom Left
  drawLineStrip(topLeft, botLeft, white);
}

void drawPlayer(void) {
  // Using disk drawing method in tutorial 9
  glPushMatrix();
    setColoringMethod(player.playerColor);
    glTranslatef(player.currPos.x, player.currPos.y, 0.0);
    glRotatef(player.rotation, 0.0, 0.0, 1.0);
    glScalef(player.size.x, player.size.y, player.size.z);
    gluDisk(player.quadric, 0.0, player.radius, player.slices, player.loops);
  glPopMatrix();
}

void drawGuide() {
  // Only show guide pre launch
  if (!global.go) {
    // Calculating end point
    vec2 end;
    end.x = cos(degreesToRadians(player.rotation)) * player.velocity.x;
    end.y = sin(degreesToRadians(player.rotation)) * player.velocity.y;

    // Scale size
    end.x *= player.guideSize;
    end.y *= player.guideSize;

    // Attach guide to initialPosition
    end.x += player.initPos.x;
    end.y += player.initPos.y;

    drawLineStrip(player.initPos, end, red);
  }
}

void drawObstacles(void) {
  // Using disk drawing method in tutorial 9
  glPushMatrix();
    setColoringMethod(peg.color);
    glTranslatef(peg.pos.x, peg.pos.y, 0.0);
    glScalef(peg.size.x, peg.size.y, peg.size.z);
    gluDisk(peg.quadric, 0.0, peg.radius, peg.slices, peg.loops);
  glPopMatrix();
}

// ##### Game Logic implemtation ######
void resetPlayer() {
  // Reduce life count by 1

  // Reset player position to start of level
  player.currPos = player.initPos;

  // Reset time and allow player to be able to launch again
  global.elapsedTime = 0.0;
  global.go = !global.go;
}

// ##### Movement and Collision detection #####
void integrate() {
  // Calculate movement of ball
  static float t = 0.0, h;

  // // Calculate time increment
  h = global.elapsedTime - t;
  t = global.elapsedTime;

  // player.currPos.x += h * player.velocity.x;
  player.currPos.y -= h * player.velocity.y;

  printf("Time: %0.2f\n", global.elapsedTime);
  // printf("Position: %0.2f,%0.2f\n", player.currPos.x, player.currPos.y);
}

void bruteForceCollision() {
  // float distance, sum_radii;
  // float m1, m2, M;
  // float v1i, v2i, v1f, v2f;
  // int i, j;
  //
  // // Kinetic energy
  // float k1, k2;

  // Values to compare for collision with walls
  float leftCollide = player.currPos.x - player.radius;
  float rightCollide = player.currPos.x + player.radius;
  float topCollide = player.currPos.y + player.radius;

  // 1. Collisions between player ball and pegs

  // 2. Collisions against level wall
  if (leftCollide <= left || rightCollide >= right || topCollide >= top) {
    player.velocity.x *= -1;
    player.velocity.y *= -1;
  }
}

// ##### Main functions #####
void update(void) {
  if (!global.go)
    return;

  global.elapsedTime = glutGet(GLUT_ELAPSED_TIME) / (float)milli - global.startTime;

  // Reset when fall out of level field
  if(player.currPos.y < bottom)
    resetPlayer();

  // Move player
  integrate();
  // Check for collisions
  bruteForceCollision();

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

  // Draw window encasing level
  drawLevelWindow();
  drawPlayer();
  drawObstacles();

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
      // if(!global.go)
        global.go = !global.go;
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
