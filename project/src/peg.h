#pragma once

#include "util.h"

typedef enum { orange, blue } type;

const float MASS = 1.0;
const float ELASTICITY = 1.0;
const glm::vec3 SIZE = { 0.7, 0.7, 0.7 };

const float SEGMENTS = 8.0;
const float RADIUS = 0.05;

const float PEG_NUM_VERTICES = 16.0;  // Segments * 2
const float PEG_NUM_INDICES = 16.0;   // Indices = Vertices

const glm::vec3 BLUE = { 0.0, 0.25, 1.0 };
const glm::vec3 BLUE_HIT = { 0.0, 1.0, 1.0 };
const glm::vec3 ORANGE = { 1.0, 0.25, 0.0 };
const glm::vec3 ORANGE_HIT = { 1.0, 1.0, 0.0 };

class Peg
{
  protected:
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec3 size;
    glm::vec3 color;
    float mass;
    float elasticity;
    float radius;
    float segments;
    float collision_radius;
    bool hit;
    bool clear;
    bool empty;
    type peg_type;
    Buffer peg;

  public:
    // Constructor
    Peg(type);
    // Methods that require to be implemented
    virtual void init_peg(void) = 0;
    virtual void draw_peg(void) = 0;
    virtual int peg_hit(void) = 0;
    virtual int peg_clear(void) = 0;

    // VBOs
    void init_vbo(void);
    void bind_vbo(void);
    // void unbind_vbo(void);

    // Getters
    bool is_hit(void);
    bool is_clear(void);
    bool is_empty(void);
    bool is_orange(void);
    glm::vec2 get_position(void);
    glm::vec2 get_velocity(void);
    float get_mass(void);
    float get_collision_radius(void);

    // Setters
    void set_position(float, float);
    void set_empty(void);
};
