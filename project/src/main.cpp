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

  0, // balls
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

// ##### Misc functions #####
float degreesToRadians(float degrees) {
  float radians = degrees * (M_PI / 180);
  return radians;
}

float roundDownFloat(int value) {
  const int decimalPlace = 10;  // Round to 1 decimal place (in the tens)
  float result;
  result = floorf(value * decimalPlace) / decimalPlace;

  return result;
}

// ##### Initialization #####
void initPlayer(void) {
  // Default color is greyish
  player.playerColor = grey;

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
void resetPlayer() {
  // Reduce life count by 1

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
  static float lastT = -1.0;
  float t, dt;  // time, delta time

  if (!global.go) {
    lastT = -1.0;
    return;
  }

  t = glutGet(GLUT_ELAPSED_TIME) / (float)milli - global.startTime;

  if (lastT < 0) {
    lastT = t;
    return;
  }

  dt = t - lastT;

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

  // Draw window encasing level
  drawLevelWindow();
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
