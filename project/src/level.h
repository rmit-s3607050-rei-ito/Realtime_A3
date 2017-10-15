#pragma once

#include <memory>

#include "util.h"
#include "catcher.h"
#include "launcher.h"
#include "player.h"
#include "peg.h"
#include "normal.h"

// Level itself, walls and all present objects
const int LEVEL_NUM_VERTICES = 4;   // For default type level with 3 walls
const int LEVEL_NUM_INDICES = 6;    // 2 sets of indices per wall, 3 walls
const float WALL_GAP = 0.075;
const float WALL_REBOUND = -1.0;
const int NUM_BALLS = 8;
const int WIDTH = 15;
const int HEIGHT = 6;

class Level {
  // All classes present in level
  Player player;
  Launcher launcher;
  Catcher catcher;
  Peg * pegs[HEIGHT][WIDTH];

  // VBOs
  Buffer wall;

  // Positioning and Color
  glm::vec2 top_left, top_right;
  glm::vec2 bot_left, bot_right;
  glm::vec3 wall_color;

  // Level parameters
  int balls;
  int score;
  int oranges;

  public:
    Level(void) {};
    // VBO related functions
    void init_vbo(void);
    void bind_vbo(void);
    // void unbind_vbo(void);

    // Level functionality
    void init_level(void);
    void draw_walls(void);
    void draw_level(void);
    bool reset_player(void);

    // Pegs
    void init_pegs(void);
    void draw_pegs(void);

    // Collisions
    void check_all_collisions(void);
    void check_wall_collision(void);

    // Getters
    int get_balls(void);
    int get_score(void);

    // Functions from attached classes
    // Catcher
    void move_catcher(float);
    // Player
    void rotate_launcher(direction);
    void set_launch(void);
    void integrate(float dt);
};
