#pragma once

#include "util.h"

const float MASS = 1.0;
const float ELASTICITY = 1.0;
const glm::vec3 SIZE = { 0.7, 0.7, 0.7 };

const glm::vec3 BLUE = { 0.0, 0.5, 1.0 };
const glm::vec3 BLUE_HIT = { 0.0, 1.0, 1.0 };
const glm::vec3 ORANGE = { 1.0, 0.5, 0.0 };
const glm::vec3 ORANGE_HIT = { 1.0, 1.0, 0.0 };

class Peg
{
  protected:
    glm::vec2 position;
    glm::vec2 velocity;
    float mass;
    float elasticity;
    glm::vec3 size;
    glm::vec3 color;
    float collision_radius;
    bool hit;
    bool clear;
    bool empty;
    bool orange;

  public:
    // Constructor
    Peg(void);
    virtual void init_peg(void) = 0;
    virtual void draw_peg(void) = 0;
    virtual int peg_hit(void) = 0;
    virtual int peg_clear(void) = 0;
    bool is_hit(void);
    bool is_clear(void);
    bool is_empty(void);
    bool is_orange(void);
    glm::vec2 get_position(void);
    glm::vec2 get_velocity(void);
    float get_mass(void);
    float get_collision_radius(void);
    void set_position(float, float);
    void set_empty(void);
    void set_orange(void);
};
