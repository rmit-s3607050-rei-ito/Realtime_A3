#include "util.h"

float degrees_to_radians(float degrees)
{
  float radians = degrees * (M_PI / 180);
  return radians;
}

// #################### Immediate mode drawing ####################
void draw_line_strip(glm::vec2 start, glm::vec2 end, glm::vec3 color)
{
  glPushMatrix();
    set_coloring_method(color);
    glBegin(GL_LINE_STRIP);
      glVertex2f(start.x, start.y);
      glVertex2f(end.x, end.y);
    glEnd();
  glPopMatrix();
}

void draw_square(glm::vec2 tL, glm::vec2 tR, glm::vec2 bR, glm::vec2 bL, glm::vec3 color)
{
  glPushMatrix();
    set_coloring_method(color);
    glBegin(GL_POLYGON);
      glVertex2f(tL.x, tL.y);
      glVertex2f(tR.x, tR.y);
      glVertex2f(bR.x, bR.y);
      glVertex2f(bL.x, bL.y);
    glEnd();
  glPopMatrix();
}

void draw_circle(float segments, float radius)
{
  glm::vec2 pos;
  float theta;

  glBegin(GL_POLYGON);
    for (int i = 0; i <= segments * 2.0; i++) {
      theta = i / segments * M_PI;
      pos.x = radius * cosf(theta);
      pos.y = radius * sinf(theta);

      glVertex3f(pos.x, pos.y, 0.0);
    }
  glEnd();
}

// #################### VBO drawing + related functions ####################
void init_vbo_square(Buffer * buffer, glm::vec2 topLeft, glm::vec2 topRight,
                     glm::vec2 botRight, glm::vec2 botLeft, glm::vec3 color)
{
  int numVerts = buffer->num_verts;
  int numInds = buffer->num_inds;

  // Allocate memory
  buffer->verts = (Vertex *) calloc(buffer->num_verts, sizeof(Vertex));
  buffer->inds = (unsigned int*) calloc(buffer->num_inds, sizeof(int));

  // Coordinates for cannon
  buffer->verts[0].pos = topLeft;
  buffer->verts[1].pos = topRight;
  buffer->verts[2].pos = botRight;
  buffer->verts[3].pos = botLeft;

  // Colors for coordinates for square shape (assuming all are of same color)
  for (int i = 0; i < numVerts; i++)
    buffer->verts[i].color = color;
  // Indices to draw points for square. Indices match coord order
  for (int i = 0; i < numInds; i++)
    buffer->inds[i] = i;
}

void init_vbo_circle(Buffer * buffer, float segments, float radius, glm::vec3 color)
{
  float theta;
  glm::vec2 vertex;

  // Allocate memory
  buffer->verts = (Vertex *) calloc(buffer->num_verts, sizeof(Vertex));
  buffer->inds = (unsigned int*) calloc(buffer->num_inds, sizeof(int));

  // Loop through
  for (int i = 0; i < segments * 2.0; i++) {
    theta = i / segments * M_PI;
    vertex.x = radius * cosf(theta);
    vertex.y = radius * sinf(theta);

    // Instead of drawing circle, store vertices, color and indices in arrays
    buffer->verts[i].pos = vertex;
    buffer->verts[i].color = color;
    buffer->inds[i] = i;
  }
}

void draw_vbo_shape(Buffer *buffer, GLenum mode, glm::vec3 color)
{
  glPushMatrix();
    set_material_color(color);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ibo);

    glVertexPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
    glColorPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(sizeof(glm::vec2)));
    glDrawElements(mode, buffer->num_inds, GL_UNSIGNED_INT, 0);
  glPopMatrix();
}

void generate_buffers(Buffer * buffer, float numVertices, float numIndices)
{
  // Generate buffers for verticies and indices
  glGenBuffers(1, &buffer->vbo);
  glGenBuffers(1, &buffer->ibo);

  // Allocate number of verticies and indices
  buffer->num_verts = numVertices;
  buffer->num_inds = numIndices;
}

void clear_buffers(Buffer * buffer)
{
  // Disable enabled client states
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);

  // Free memory allocated to indices and verticies
  free(buffer->inds);
  free(buffer->verts);

  // Empty buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void set_vbo_buffer_data(Buffer * buffer)
{
  // Vertices
  glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
  glBufferData(GL_ARRAY_BUFFER, buffer->num_verts * sizeof(Vertex),
               buffer->verts, GL_STATIC_DRAW);
  // Indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer->num_inds * sizeof(unsigned int),
               buffer->inds, GL_STATIC_DRAW);
}
