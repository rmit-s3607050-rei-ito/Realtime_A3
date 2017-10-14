#include "level.h"

struct Global {
  bool go;
  bool wireframe;

  float dt;
  float bounce;
};

Global global;
Level level;

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

void init(void) {
  // Initialize level
  level.init_level();

  // Initialize global variables
  global.go = false;
  global.wireframe = false;

  global.dt = 0.0;
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

void setColoringMethod(glm::vec3 color) {
  if(global.wireframe)
    glColor3fv(&color.x);
  else
    glMaterialfv(GL_FRONT, GL_DIFFUSE, &color.r);
}

// ##### Game Logic implementation ######
// void clearPegs(void) {
//   // Remove all hit pegs and add to score per hit peg
//   for(int row = 0; row < HEIGHT; row++) {
//     for (int col = 0; col < WIDTH; col++) {
//       if (!pegs[row][col].clear && !pegs[row][col].empty) {
//         if (pegs[row][col].hit) {
//           pegs[row][col].clear = true;
//           global.score += 5;
//         }
//       }
//     }
//   }
// }

// void resetPlayer(Player *player) {
//   // Check whether player landed in catcher, if not reduce ball count by 1
//   if (!catcher.caught_player(&global.player))
//     global.balls--;
//
//   // clearPegs();
//
//   // Reset player position to start of level and reset velocity
//   player->currPos = player->initPos;
//   player->currVel = player->initVel;
//
//   // Allow player to launch again
//   global.go = !global.go;
// }

// ##### Collision detection #####
// void bruteForceCollision() {
  // 2. Collision with sides of catcher
  // catcher.catcher_collide(&global.player);

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
  //           global.score += 1;
  //
  //         pegs[row][col].hit = true;
  //       }
  //     }
  //   }
  // }
// }

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
  snprintf(buffer, sizeof buffer, "Balls: %d", level.get_balls());
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
  // Frame time
  glRasterPos2i(w-90, h-22);
  snprintf(buffer, sizeof buffer, "Score: %d", level.get_score());
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
  global.dt = getDeltaTime();

  // Constantly move catcher
  level.move_catcher(global.dt);

  // Move player and check for collisions only when launching
  if (global.go) {
    level.integrate(global.dt);
    level.check_all_collisions();

    // Check when player falls out of arena and is to be resetted
    if(level.reset_player())
      global.go = !global.go;
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
  level.draw_level();

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
      if (!global.go)
        level.rotate_launcher(LEFTWARDS);
      break;
    case 'd': // right
      if (!global.go)
        level.rotate_launcher(RIGHTWARDS);
      break;

    // Toggle wireframe/filled mode
    case '1':
      global.wireframe = !global.wireframe;
      break;

    case ' ': // 'space' = launch ball
      if(!global.go && level.get_balls() > 0) {
        // Set velocity of x and y depending on direction rotated to launch
        level.set_launch();
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
