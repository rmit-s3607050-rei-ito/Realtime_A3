#include "level.h"

struct Global {
  bool game_end;
  bool go;
  bool wireframe;

  float dt;
};

Global global;
Level level;

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

void set_coloring_method(glm::vec3 color) {
  // Only used for guide now, VBOs, automatically store color for wireframe mode
  if(global.wireframe)
    glColor3fv(&color.x);
  else
    glMaterialfv(GL_FRONT, GL_DIFFUSE, &color.r);
}

void set_material_color(glm::vec3 color) {
  if (!global.wireframe)
    glMaterialfv(GL_FRONT, GL_DIFFUSE, &color.r);
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
  // Ball Count
  glRasterPos2i(20, h-22);
  snprintf(buffer, sizeof buffer, "Balls: %d", level.get_balls());
  for (bufp = buffer; *bufp; bufp++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
  // Score tracking
  glRasterPos2i(w-100, h-22);
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
    if(level.reset_player()) {
      global.go = !global.go;
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
