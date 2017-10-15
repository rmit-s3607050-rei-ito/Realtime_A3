/* COSC1226 Real-Time Rendering and 3D Games Programming - Assignment 3
 * Assignment done as a pair
 * -----------------------------------------------------------------------------
 * Name             : Rei Ito
 * Student Number   : s3607050
 * -----------------------------------------------------------------------------
 * Name             : Pacific Thai
 * Student Number   : s3429648
 * -----------------------------------------------------------------------------
 */

#pragma once

#include "util.h"
#include "catcher.h"
#include "launcher.h"
#include "player.h"
#include "peg.h"
#include "normal.h"
#include "square.h"
#include "triangle.h"
#include "pentagon.h"
#include "hexagon.h"

// Level itself, walls and all present objects
const int LEVEL_NUM_VERTICES = 4;   // For default type level with 3 walls
const int LEVEL_NUM_INDICES = 6;    // 2 sets of indices per wall, 3 walls

const float WALL_GAP = 0.075;
const float WALL_REBOUND = -1.0;

const int NUM_BALLS = 8;

const int WIDTH = 15;
const int HEIGHT = 6;

const int SHAPES = 9;

class Level
{
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
  int num_orange_pegs;

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
    void clear_hit_pegs(void);
    bool reset_player(void);
    bool game_end(void);

    // Pegs
    void init_pegs(void);
    void draw_pegs(void);

    // Collisions
    void check_all_collisions(void);
    void check_peg_collisions(void);
    void check_wall_collision(void);

    // Getters
    int get_balls(void);
    int get_score(void);
    int get_num_orange_pegs(void);

    // Functions from attached classes
    void move_pegs(float);
    // Catcher
    void move_catcher(float);
    // Player
    void rotate_launcher(direction);
    void set_launch(void);
    void integrate(float dt);
};
