// Utility file, contains constants and global variables
#pragma once

#define GL_GLEXT_PROTOTYPES
// SHADER STUFF
// #include "shaders.h"

// Required libraries
#include <iostream>
#include <random>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

// #################### Definitions ####################
// PI: 3.14159265359
#ifndef M_PI
  #define M_PI 4.0 * atan(1.0)
#endif
// VBO buffer access offsetting
#define BUFFER_OFFSET(i) ((void *) (i))

// #################### Reflection and rotating launch ####################
typedef enum { X_REFLECTION, Y_REFLECTION } reflection;
typedef enum { LEFTWARDS, RIGHTWARDS } direction;

// #################### VBO struct data definitions ####################
typedef struct {
  glm::vec2 pos;
  glm::vec3 color;
} Vertex;

typedef struct {
  Vertex *verts;
  unsigned int *inds;
  size_t num_verts, num_inds;
  unsigned vbo, ibo;
} Buffer;

// #################### Constants ####################
// const float gravity = -9.8;   // Gravity
const float gravity = -2.0;   // Fake Gravity
const float milli = 1000.0;   // One second

// Window dimensions
const float WINDOW_X = 600.0;
const float WINDOW_Y = 700.0;
const float WINDOW_POS_X = 400.0;
const float WINDOW_POS_Y = 0.0;

// Shared variables:
// Scoring
const int CATCHER_CATCH_SCORE = 10;

// Colors
const glm::vec3 WHITE = { 1.0, 1.0, 1.0 };
const glm::vec3 GREY = { 0.2, 0.2, 0.2 };
const glm::vec3 RED = { 1.0, 0.0, 0.0 };
const glm::vec3 YELLOW = { 1.0, 1.0, 0.0 };
const glm::vec3 BROWN = { 0.55, 0.27, 0.07 };
const glm::vec3 DARK_GREY = { 0.1, 0.1, 0.1 };
// Level boundaries
const float LEFT = -0.90;
const float BOTTOM = -1.0;
const float TOP = 0.90;
const float RIGHT = 0.90;
// Positioning and Rotation
const float PLAYER_START_POS_Y = 0.85;
const float STARTING_ROTATION = -90.0;
// Drawing
const int SQUARE_NUM_VERTICES = 4;
const int SQUARE_NUM_INDICES = 4;

// Misc functions
float degrees_to_radians(float);
void set_coloring_method(glm::vec3);  // Set color dependant on filled/wireframe mode
void set_material_color(glm::vec3);

// Immediate mode drawing
void draw_line_strip(glm::vec2, glm::vec2, glm::vec3);
void draw_square(glm::vec2, glm::vec2, glm::vec2, glm::vec2, glm::vec3);
void draw_circle(float, float);

// VBO functions
void init_vbo_square(Buffer *, glm::vec2, glm::vec2, glm::vec2, glm::vec2,
  glm::vec3);
void init_vbo_circle(Buffer *, float, float, glm::vec3);
void init_vbo_triangle(Buffer *, glm::vec2, glm::vec2, glm::vec2, glm::vec3);
void init_vbo_polygon(Buffer *, int, float, glm::vec3);

void draw_vbo_shape(Buffer *, GLenum, glm::vec3);
void generate_buffers(Buffer *, float, float);
void clear_buffers(Buffer *);
void set_vbo_buffer_data(Buffer *);
