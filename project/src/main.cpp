#include "level.h"

struct Global {
  bool game_end;
  bool go;
  bool wireframe;
  bool win;

  int want_redisplay;
  SDL_Window *window;

  float dt;
};

Global global;
Level level;

// ##### Misc functions #####
static float getDeltaTime(void)
{
  static float t1 = -1.0;

  if (t1 == -1)
    t1 = SDL_GetTicks();

  float t2 = SDL_GetTicks();
  float dt = (t2 - t1) / milli;
  t1 = t2;
  return dt;
}

void post_redisplay()
{
  global.want_redisplay = 1;
}

void quit(int code)
{
  SDL_DestroyWindow(global.window);
  SDL_Quit();
  exit(code);
}

void sys_shutdown()
{
  SDL_Quit();
}

void init(void)
{
  glShadeModel(GL_SMOOTH);

  // Initialize level
  level.init_level();

  // Initialize global variables
  global.game_end = false;
  global.go = false;
  global.wireframe = false;
  global.win = false;
  global.want_redisplay = 1;
  global.dt = 0.0;
}

// ##### Drawing and display #####
void set_render_mode(void)
{
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

void set_coloring_method(glm::vec3 color)
{
  // Only used for guide now, VBOs, automatically store color for wireframe mode
  if(global.wireframe)
    glColor3fv(&color.x);
  else
    glMaterialfv(GL_FRONT, GL_DIFFUSE, &color.r);
}

void set_material_color(glm::vec3 color)
{
  if (!global.wireframe)
    glMaterialfv(GL_FRONT, GL_DIFFUSE, &color.r);
}

void display_osd()
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

  glColor3fv(&YELLOW.x);
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

  // Print win/loss message when game ends
  if (global.game_end) {
    if (global.win) {
      glRasterPos2i((w/2) - 120, h/2);
      snprintf(buffer, sizeof buffer, "CONGRATULATIONS: YOU WIN!");
    } else {
      glRasterPos2i((w/2) - 90, h/2);
      snprintf(buffer, sizeof buffer, "GAME OVER: YOU LOSE!");
    }
    for (bufp = buffer; *bufp; bufp++)
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
  }

  glPopMatrix();  /* Pop modelview */
  glMatrixMode(GL_PROJECTION);

  glPopMatrix();  /* Pop projection */
  glMatrixMode(GL_MODELVIEW);

  glPopAttrib();
}

// ##### Main functions #####
void update(void)
{
  global.dt = getDeltaTime();

  // Constantly move catcher
  level.move_catcher(global.dt);

  // Move player and check for collisions only when launching
  if (global.go && !global.game_end) {
    level.integrate(global.dt);
    level.check_all_collisions();

    // Check when player falls out of arena and is to be resetted
    if(level.reset_player()) {
      // Check whether the game ended or not
      global.game_end = level.game_end();
      if(!global.game_end)
        global.go = !global.go; // Continue, game hasnt ended
      else {
        // Game has ended, check whether player won or lost
        if (level.get_num_orange_pegs() == 0)
          global.win = true;
        else
          global.win = false;
      }
    }
  }

  post_redisplay();
}

void display(void)
{
  int err;

  glLoadIdentity(); // Set initial matrix
  glPushMatrix();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Apply render mode
  set_render_mode();

  // Draw level objects
  level.draw_level();

  glPopMatrix();

  display_osd();

  SDL_GL_SwapWindow(global.window);

  if ((err = glGetError()) != GL_NO_ERROR)
    printf("display: %s\n", gluErrorString(err));
}

void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0, 1.0, -1.0, 1.0, -1, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

// SDL's version keyboard events
void key_down(SDL_KeyboardEvent *e)
{
  switch (e->keysym.sym) {
    // Rotate launch position
    case SDLK_a: // left
      if (!global.go)
        level.rotate_launcher(LEFTWARDS);
      break;
    case SDLK_d: // right
      if (!global.go)
        level.rotate_launcher(RIGHTWARDS);
      break;
    // Toggle wireframe/filled mode
    case SDLK_w:
      global.wireframe = !global.wireframe;
      break;
    case SDLK_SPACE: // 'space' = launch ball
      if(!global.go && level.get_balls() > 0) {
        // Set velocity of x and y depending on direction rotated to launch
        level.set_launch();
        global.go = !global.go;
      }
      break;

    // Quit Application
    case SDLK_q:
      quit(0);
      break;

    default:
      break;
  }
}

void event_dispatcher()
{
  SDL_Event e;

  // Handle events
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      // Keyboard related events
      case SDL_KEYDOWN:
        key_down(&e.key);
        break;
      // Window related events
      case SDL_WINDOWEVENT:
        switch (e.window.event) {
          case SDL_WINDOWEVENT_SHOWN:
            break;
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            break;
          case SDL_WINDOWEVENT_RESIZED:
            if (e.window.windowID == SDL_GetWindowID(global.window)) {
              SDL_SetWindowSize(global.window, e.window.data1, e.window.data2);
              reshape(e.window.data1, e.window.data2);
              post_redisplay();
            }
            break;
          case SDL_WINDOWEVENT_CLOSE:
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }

  post_redisplay();
}

void main_loop()
{
  // Infinite loop reading events
  while (1) {
    event_dispatcher();

    if (global.want_redisplay) {
      display();
      global.want_redisplay = 0;
    }
    update();
  }
}

int init_graphics()
{
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  global.window =
    SDL_CreateWindow("Peggle",
         SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
         WINDOW_X, WINDOW_Y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (!global.window) {
    fprintf(stderr, "%s:%d: create window failed: %s\n",
      __FILE__, __LINE__, SDL_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_GLContext mainGLContext = SDL_GL_CreateContext(global.window);
  if (mainGLContext == 0) {
    fprintf(stderr, "%s:%d: create context failed: %s\n",
      __FILE__, __LINE__, SDL_GetError());
    exit(EXIT_FAILURE);
  }

  return 1;
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitWindowSize(WINDOW_X, WINDOW_Y);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutCreateWindow("GLUT OSD");

  glewInit();
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
    fprintf(stderr, "%s:%d: unable to init SDL: %s\n",
      __FILE__, __LINE__, SDL_GetError());
    exit(EXIT_FAILURE);
  }
  if (!init_graphics()) {
      SDL_Quit();
      return EXIT_FAILURE;
  }

  init();
  int w, h;
  SDL_GetWindowSize(global.window, &w, &h);
  reshape(w, h);
  atexit(sys_shutdown);
  main_loop();

  return EXIT_SUCCESS;
}
