#pragma once

#include "util.h"
#include "catcher.h"
#include "launcher.h"
#include "player.h"

const float WALL_GAP = 0.075;
const float WALL_REBOUND = -1.0;
const int NUM_BALLS = 8;

class Level {
  Player player;
  Launcher launcher;
  Catcher catcher;

  glm::vec2 top_left, top_right;
  glm::vec2 bot_left, bot_right;
  glm::vec3 wall_color;

  int balls;
  int score;

  public:
    Level(void) {};
    // Main functions
    void init_level(void);
    void draw_level(void);
    bool reset_player(void);

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
