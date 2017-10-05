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

  270.0,                 // rotation
  1.0,                   // rotationInc
  180.0,                 // minRotation
  360.0,                 // maxRotation

  1.0,                   // velocity
  0.05,                  // radius
  0.0,                   // mass
  0.0,                   // elasticity

  0,                     // quadric
  10,                    // slices
  3,                     // loops
  { 0.65, 0.65, 1.0 },   // size
  { 0.0, 0.0, 0.0, 0.0 } // color
};

// ##### Other functions #####
float degreesToRadians(float degrees) {
  float radians = degrees * (M_PI / 180);
  return radians;
}

// ##### Initialization #####
void initPlayer(void) {
  // Default color is greyish
  player.playerColor = grey;

  // Positioning
  player.initPos = (coordinate) { 0.0, 0.87 };
  player.currPos = player.initPos;

  // Initialize quadric for rendering
  player.quadric = gluNewQuadric();
}

void init(void) {
  initPlayer();
}

// ##### Drawing functions #####
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

void drawLineStrip(coordinate start, coordinate end, color4f color) {
  glPushMatrix();
    setColoringMethod(color);
    // Draw the line
    glBegin(GL_LINE_STRIP);
      glVertex2f(start.x, start.y);
      glVertex2f(end.x, end.y);
    glEnd();
  glPopMatrix();
}

void drawVelocityVector() {
  // Only draw pre launch
  if (!global.go) {
    // Calculating end point
    coordinate end;
    end.x = cos(degreesToRadians(player.rotation)) * player.velocity;
    end.y = sin(degreesToRadians(player.rotation)) * player.velocity;

    // Scale size
    end.x *= player.size.x;
    end.y *= player.size.y;

    // Attach velocity vector to initialPosition
    end.x += player.initPos.x;
    end.y += player.currPos.y;

    drawLineStrip(player.initPos, end, red);
  }
}

// ##### Main functions #####
void update(void) {
  if (!global.go)
    return;

  global.elapsedTime = glutGet(GLUT_ELAPSED_TIME) / (float)milli - global.startTime;

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
  drawVelocityVector();

  // Draw window encasing level
  drawLevelWindow();
  drawPlayer();

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
    case 'w':
      global.wireframe = !global.wireframe;
      break;

    case ' ': // 'space' = launch ball
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
