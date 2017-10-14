// #include "player.h"
// #include "level.h"
// #include "util.h"
#include "globals.h"

Globals globals;

Obstacle pegs[HEIGHT][WIDTH];

// int numPoints = 0;
// const int MAX_POINTS = 1000;
// const int MAX_PREDICTIONS = 2;
// vec2 prediction[MAX_POINTS];

// ##### Misc functions #####
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
  float leftLimit = LEFT + 0.1;
  float rightLimit = RIGHT;
  float bottomLimit = BOTTOM + 0.35;
  float topLimit = TOP;

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

// ##### Drawing and display #####
void setRenderMode(void) {
  if(globals.wireframe) {
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
  if(globals.wireframe)
    glColor4fv(&color.r);
  else
    glMaterialfv(GL_FRONT, GL_DIFFUSE, &color.r);
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
void clearPegs(void) {
  // Remove all hit pegs and add to score per hit peg
  for(int row = 0; row < HEIGHT; row++) {
    for (int col = 0; col < WIDTH; col++) {
      if (!pegs[row][col].clear && !pegs[row][col].empty) {
        if (pegs[row][col].hit) {
          pegs[row][col].clear = true;
          globals.score += 5;
        }
      }
    }
  }
}

void resetPlayer(void) {
  // Range for catcher, if land between these values ball is not lost
  float catcherStart = catcher.position.x + catcher.leftTR.x;
  float catcherEnd = catcher.position.x + catcher.rightTL.x;

  // Reduce life count by 1 if ball didnt end up in the catcher
  if (globals.player.currPos.x < catcherStart || globals.player.currPos.x > catcherEnd)
    globals.balls--;

  // Reset player position to start of level and reset velocity
  globals.player.currPos = globals.player.initPos;
  globals.player.currVel = globals.player.initVel;
}

// ##### Collision detection #####
void bruteForceCollision() {
  // Radius of player, with scaling applied
  float playerRadius = (globals.player.radius * globals.player.size.x);

  // Values to compare for collision with ball
  float playerLeft = globals.player.currPos.x - playerRadius;
  float playerRight = globals.player.currPos.x + playerRadius;
  float playerTop = globals.player.currPos.y + playerRadius;
  float playerBottom = globals.player.currPos.y - playerRadius;

  // 2. Collisions against level wall
  wallCollide(&globals.player, playerLeft, playerRight, playerTop);

  // 3. Collision with sides of catcher
  catcherCollide(&globals.catcher, &globals.player, playerBottom);

  // 3. Collisions against pegs
  // double radiusSum, radiusSumSqr, dissMagSqr;
  // vec2 diss;
  //
  // for(int row = 0; row < HEIGHT; row++) {
  //   for (int col = 0; col < WIDTH; col++) {
  //     if (!pegs[row][col].clear && !pegs[row][col].empty) {
  //       float pegRadius = (pegs[row][col].radius * pegs[row][col].size.x);
  //
  //       radiusSum = playerRadius + pegRadius;
  //       radiusSumSqr = radiusSum * radiusSum;
  //
  //       diss.x = pegs[row][col].pos.x - player.currPos.x;
  //       diss.y = pegs[row][col].pos.y - player.currPos.y;
  //
  //       dissMagSqr = (diss.x * diss.x) + (diss.y * diss.y);
  //       if (dissMagSqr <= radiusSumSqr) {
  //         float n[2], t[2], n_mag;
  //         float v1_nt[2], v2_nt[2];
  //         float m1, m2, v1i, v2i, v1f, v2f;
  //
  //         n[0] = pegs[row][col].pos.x - player.currPos.x;;
  //         n[1] = pegs[row][col].pos.y - player.currPos.y;;
  //
  //         n_mag = sqrt(n[0] * n[0] + n[1] * n[1]);
  //         n[0] /= n_mag;
  //         n[1] /= n_mag;
  //
  //         t[0] = -n[1];
  //         t[1] = n[0];
  //
  //         v1_nt[0] = n[0] * player.currVel.x + n[1] * player.currVel.y;
  //         v1_nt[1] = t[0] * player.currVel.x + t[1] * player.currVel.y;
  //         v2_nt[0] = n[0] * 0 + n[1] * 0;
  //         v2_nt[1] = t[0] * 0 + t[1] * 0;
  //
  //         m1 = player.mass;
  //         m2 = pegs[row][col].mass;
  //         v1i = v1_nt[0];
  //         v2i = v2_nt[0];
  //         v1f = (m1 - m2) / (m1 + m2) * v1i + 2.0 * m2 / (m1 + m2) * v2i;
  //         v2f = 2.0 * m1 / (m1 + m2) * v1i + (m2 - m1) / (m1 + m2) * v2i;
  //
  //         v1_nt[0] = v1f;
  //         v2_nt[0] = v2f;
  //
  //         player.currVel.x = (n[0] * v1_nt[0] + t[0] * v1_nt[1])
  //           - (n[0] * v2_nt[0] + t[0] * v2_nt[1]);
  //         player.currVel.y = (n[1] * v1_nt[0] + t[1] * v1_nt[1])
  //           - (n[1] * v2_nt[0] + t[1] * v2_nt[1]);
  //
  //         // If peg not already hit, add to score
  //         if (!pegs[row][col].hit)
  //           globals.score += 1;
  //
  //         pegs[row][col].hit = true;
  //       }
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
  snprintf(buffer, sizeof buffer, "Balls: %d", globals.balls);
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
  // Frame time
  glRasterPos2i(w-90, h-22);
  snprintf(buffer, sizeof buffer, "Score: %d", globals.score);
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
  globals.dt = getDeltaTime();

  // Constantly move catcher
  moveCatcher(&globals.catcher, globals.dt);

  // Move player and check for collisions only when launching
  if (globals.go) {
    integrate(globals.dt, &globals.player);
    bruteForceCollision();

    // Reset when fall out of level field
    if(globals.player.currPos.y < BOTTOM) {
      resetPlayer();
      clearPegs();
      // Allow player to launch again
      globals.go = !globals.go;
    }
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
  drawScene(&globals);
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
      if (!globals.go && globals.player.rotation > globals.player.minRotation) {
        globals.player.rotation -= globals.player.rotationInc;
        // prediction = calculateGuidePoints();
      }
      break;
    case 'd': // right
      if (!globals.go && globals.player.rotation < globals.player.maxRotation) {
        globals.player.rotation += globals.player.rotationInc;
        // prediction = calculateGuidePoints();
      }
      break;

    // Toggle wireframe/filled mode
    case '1':
      globals.wireframe = !globals.wireframe;
      break;

    case ' ': // 'space' = launch ball
      if(!globals.go && globals.balls > 0) {
        // Set velocity of x and y depending on direction rotated to
        setLaunch(&globals.player);
        globals.go = !globals.go;
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

  glutDisplayFunc(display);
  glutIdleFunc(update);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);

  initObstacles();
  initGlobals(&globals);

  glutMainLoop();

  return 0;
}
