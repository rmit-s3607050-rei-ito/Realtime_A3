#include "util.h"

float degreesToRadians(float degrees) {
  float radians = degrees * (M_PI / 180);
  return radians;
}

void drawLineStrip(glm::vec2 start, glm::vec2 end, color4f color) {
  glPushMatrix();
    setColoringMethod(color);
    glBegin(GL_LINE_STRIP);
      glVertex2f(start.x, start.y);
      glVertex2f(end.x, end.y);
    glEnd();
  glPopMatrix();
}

void drawSquare(glm::vec2 topL, glm::vec2 topR, glm::vec2 botR, glm::vec2 botL, color4f color) {
  glPushMatrix();
    setColoringMethod(color);
    glBegin(GL_POLYGON);
      glVertex2f(topL.x, topL.y);
      glVertex2f(topR.x, topR.y);
      glVertex2f(botR.x, botR.y);
      glVertex2f(botL.x, botL.y);
    glEnd();
  glPopMatrix();
}

void drawCircle(float segments, float radius) {
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
