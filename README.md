README
COSC1226 Real-Time Rendering and 3D Games Programming Assignment 3

AUTHORS
Pacific Thai - s3429648
Rei Ito - s3607050

CONTRIBUTORS
Code from throughout the semester used

FILES
Makefile
src/(*.cpp, *.h)
obj/(*.o, *.d)

INSTALL
- make
- ./peggle

BUGS
- Collision reaction isn't completely accurate, causing the player to get stuck
  or rotate around the pegs.
- Unsure of memory leaks since we didn't have enough time to check.
- We tried to convert our project from c to c++ object oriented programming and
  we feel it's quite messy.
- When the guide is rotated to the most left and right it disappears.

INCOMPLETE
- The guide only shows the initial trajectory.
- Some shader code exists but couldn't get around to it.
- SDL conversion is partially complete but the OSD and other parts of the
  program are still using GL.
- Only brute force detection was used.
- Toggles for various functions (wasn't specified to do so, so we didn't put
  it in) except for wireframe (w).
