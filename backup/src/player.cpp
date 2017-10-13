
//#include "player.h"

// Player player = {
//   { 0.0, 0.0 }, // initPos
//   { 0.0, 0.0 }, // currPos
//
//   0.0,          // velocity
//   0.05,         // radius
//   0.0,          // mass
//   0.0,          // elasticity
//
//   8.0,            // segments
//   { 0.0, 0.0, 0.0, 0.0 } // color
// };

// void initPlayer(void) {
//   // Default color is greyish
//   player.playerColor = grey;
//
//   // Positioning
//   player.initPos = (coordinate) { 0.0, 0.85 };
//   player.currPos = player.initPos;
// }

// void drawPlayer(void) {
//   glColor4fv(&player.playerColor.r);
//   float x, y;
//   float theta;
//
//   glBegin(GL_LINE_LOOP);
//   for (int i = 0; i <= player.segments * 2.0; i++) {
//     theta = i / player.segments * M_PI;
//     x = player.radius * cosf(theta); // y = r * cos(theta)
//     y = player.radius * sinf(theta); // x = r * sin(theta)
//
//     // Adjust position when moving
//     x += player.currPos.x;          y += player.currPos.y;
//
//     glVertex3f(x, y, 0.0);
//   }
//   glEnd();
// }
