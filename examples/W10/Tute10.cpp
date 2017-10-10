/*
 * This program shows how to do collision detection between
 * particles in two dimensions.
 *
 * Uses hard-sphere assumptions for collision dynamics.
 *
 * Both brute-force and uniform grid approaches implemented.
 *
 * Numerical integration of equations of motion.
 *
 * $Id: particles_2D.C,v 1.13 2012/09/24 00:57:22 gl Exp gl $
 */
#include <math.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <limits.h>
#include <stdlib.h>

/* Debugging controls .*/
enum debugFlags {
  debug_time,
  debug_wall,
  debug_initialise_particle,
  debug_particle,
  debug_particle_collision,
  debug_collideParticlesBruteForce,
  debug_collideParticlesUniformGrid,
  debug_collisionReactionParticles2DbasisChange,
  debug_collisionReactionParticles2DprojNormal,
  debug_framerate,
  debug_range_check,
  debug_sum_kinetic_energy,
  debug_sum_momentum,
  numDebugFlags
};
int debug[numDebugFlags] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1 };

/* Use our type so we can change precision easily. */
typedef double Real;

/* Small number to handle numerical imprecision. */
const Real epsilon = 1.0e-6;

/* Particles (particles). */
struct Particle {
  Real position[2];
  Real velocity[2];
  Real radius;
  Real mass;
  Real elasticity;
  GLUquadric *quadric;  /* For rendering. */
  int slices, loops;    /* For rendering. */
};

/* Control 1D or 2D */
const int dimension = 2;

/* Control random or explicit initial positions */
enum {
  randomly,
  explicitly
} const initialiseParticles = randomly;

/* Control collision reaction calculation */
enum ReactionCalculation {
  basisChange,
  projNormal
} reacCalc = basisChange;

const int numParticles = 100;
Particle particle[numParticles];

/* Arena. */
struct Arena {
  Real min[2], max[2];
  Real momentum[2];
} arena;

/* Rendering info. */
enum renderMode { wire, solid };
static renderMode renMode = wire;
static Real elapsedTime = 0.0, startTime = 0.0;
static const int milli = 1000;
static bool go = false;

/* Collision detection method. */
enum CollisionDetectionMethod {
  bruteForce,
  uniformGrid
};

CollisionDetectionMethod CDmethod = bruteForce;

Real random_uniform() {
  return rand()/(float)RAND_MAX;
}

void panic(const char *m) {
  printf("%s", m);
  exit(1);
}

void initialiseArena()
{
  const Real halfLength = 7.5;

  arena.min[0] = -halfLength;
  arena.min[1] = -halfLength;
  arena.max[0] = halfLength;
  arena.max[1] = halfLength;

  arena.momentum[0] = 0.0;
  arena.momentum[1] = 0.0;
}


void initialiseParticlesExplicitly()
{
  GLUquadric *quadric = gluNewQuadric();

  for (int i = 0; i < 2; i++) {
    particle[i].velocity[0] = 1.0;
    particle[i].velocity[1] = 0.0;
    particle[i].radius = 5.0;
    particle[i].mass = 1.0;
    particle[i].elasticity = 1.0;
    particle[i].quadric = quadric;
    particle[i].slices = 10;
    particle[i].loops = 3;
  }
#if 0
  particle[0].position[0] = 0.5 * arena.max[0];
  particle[1].position[0] = -0.5 * arena.max[0];
  particle[0].velocity[0] = -1.0;
  particle[1].velocity[0] = 1.0;
  particle[0].mass = 2.0;
  particle[0].radius *= sqrt(2.0);
#endif
  particle[0].position[0] = 0.5 * arena.max[0];
  particle[0].position[1] = -0.5 * arena.max[0];
  particle[1].position[0] = -0.5 * arena.max[0];
  particle[1].position[1] = -0.5 * arena.max[0];
  particle[0].velocity[0] = -1.0;
  particle[0].velocity[1] = 1.0;
  particle[1].velocity[0] = 1.0;
  particle[1].velocity[1] = 1.0;
#if 0
  particle[0].mass = 2.0;
  particle[0].radius *= sqrt(2.0);
#endif
}

void initialiseParticlesRandomly()
{
  GLUquadric *quadric = gluNewQuadric();
  const Real maxVelocity = 1.0;
  Real n[2], n_mag_sq, sum_radii, sum_radii_sq;
  bool collision, done;
  int i, j;

  for (i = 0; i < numParticles; i++) {
    particle[i].velocity[0] = (random_uniform() - 0.5) * maxVelocity;
    particle[i].velocity[1] = (random_uniform() - 0.5) * maxVelocity;
    particle[i].mass = random_uniform() * 0.1;
#if 0
    particle[i].mass = random_uniform() * 0.5;
#endif
#if 0
    particle[i].mass = 0.1;
    particle[i].radius = 0.05;
#endif
    particle[i].radius = sqrt(particle[i].mass);
    particle[i].elasticity = 1.0;
    particle[i].quadric = quadric;
    particle[i].slices = 10;
    particle[i].loops = 3;
    done = false;
    while (!done) {
      particle[i].position[0] = random_uniform() *
        (arena.max[0] - arena.min[0] - 2.0 * particle[i].radius) +
        arena.min[0] + particle[i].radius + epsilon;
      particle[i].position[1] = random_uniform() *
        (arena.max[1] - arena.min[1] - 2.0 * particle[i].radius) +
        arena.min[1] + particle[i].radius + epsilon;

      /* Check for collision with existing particles. */
      collision = false;
      j = 0;
      while (!collision && j < i) {
        sum_radii = particle[i].radius + particle[j].radius;
        sum_radii_sq = sum_radii * sum_radii;
        n[0] = particle[j].position[0] - particle[i].position[0];
        n[1] = particle[j].position[1] - particle[i].position[1];
        n_mag_sq = n[0] * n[0] + n[1] * n[1];
        if (n_mag_sq < sum_radii_sq)
          collision = true;
        else
          j++;
      }
      if (!collision)
        done = true;
    }
    if (debug[debug_initialise_particle])
      printf ("initialiseParticles: x %f y %f\n",
              particle[i].position[0], particle[i].position[1]);
  }
}

void setRenderMode(renderMode rm)
{
  /* Example of GNU C/C++ brace indentation style.  */
  if (rm == wire)
    {
      glDisable(GL_LIGHTING);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_NORMALIZE);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
  else if (rm == solid)
    {
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_NORMALIZE);
      glShadeModel(GL_SMOOTH);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

float sumKineticEnergy()
{
  Real v_sq, K;

  K = 0;
  for (int i = 0; i < numParticles; i++) {
    v_sq = particle[i].velocity[0] * particle[i].velocity[0] +
      particle[i].velocity[1] * particle[i].velocity[1];
    K += 0.5 * particle[i].mass * v_sq;
  }

  return K;
}

void sumMomentum(Real *p)
{
  p[0] = p[1] = 0;
  for (int i = 0; i < numParticles; i++) {
    p[0] += particle[i].mass * particle[i].velocity[0];
    p[1] += particle[i].mass * particle[i].velocity[1];
  }
  p[0] += arena.momentum[0];
  p[1] += arena.momentum[1];
}

void displayParticle(Particle *p, float sx, float sy, float sz)
{
  glPushMatrix();
  glScalef(sx, sy, sz);
  gluDisk(p->quadric, 0.0, p->radius, p->slices, p->loops);
  glPopMatrix();
}

void changeRenderMode(void)
{
  if (renMode == wire) {
    renMode = solid;
  } else {
    renMode = wire;
  }
  setRenderMode(renMode);
}

void displayArena(void)
{
  glBegin(GL_LINE_LOOP);
  glVertex3f(arena.min[0], arena.min[1], 0.0);
  glVertex3f(arena.max[0], arena.min[1], 0.0);
  glVertex3f(arena.max[0], arena.max[1], 0.0);
  glVertex3f(arena.min[0], arena.max[1], 0.0);
  glEnd();
}

void collideParticleWall(Particle &p, Arena &a)
{
  float dp[2];

  dp[0] = dp[1] = 0.0;
  //Arena left side and p left side
  if ((p.position[0] - p.radius) < a.min[0]) {
    p.position[0] +=
      2.0 * (a.min[0] - (p.position[0] - p.radius));
    p.velocity[0] *= -1.0;
    dp[0] += p.mass * -2.0 * p.velocity[0];
  }
  if ((p.position[1] - p.radius) < a.min[1]) {
    p.position[1] +=
      2.0 * (a.min[1] - (p.position[1] - p.radius));
    p.velocity[1] *= -1.0;
    dp[1] += p.mass * -2.0 * p.velocity[1];
  }
  if ((p.position[0] + p.radius) > a.max[0]) {
    p.position[0] -=
      2.0 * (p.position[0] + p.radius - a.max[0]);
    p.velocity[0] *= -1.0;
    dp[0] += p.mass * -2.0 * p.velocity[0];
  }
  if ((p.position[1] + p.radius) > a.max[1]) {
    p.position[1] -=
      2.0 * (p.position[1] + p.radius - a.max[1]);
    p.velocity[1] *= -1.0;
    dp[1] += p.mass * -2.0 * p.velocity[1];
  }
  arena.momentum[0] += dp[0];
  arena.momentum[1] += dp[1];
}

void collideParticlesWall(void)
{
  for (int i = 0; i < numParticles; i++) {
    if (debug[debug_wall])
      printf("%d %f %f\n",
             i, particle[i].position[0], particle[i].position[1]);
    collideParticleWall(particle[i], arena);
  }
}

void eulerStepSingleParticle(Particle &p, Real h)
{
  p.position[0] += h * p.velocity[0];
  p.position[1] += h * p.velocity[1];
}

void integrateMotionParticles(Real h)
{
  for (int i = 0; i < numParticles; i++)
    eulerStepSingleParticle(particle[i], h);
}

inline bool collisionDetectionParticles(Particle &p1, Particle &p2)
{
  Real sum_radii, sum_radii_sq, n[2], n_mag_sq;

  sum_radii = p1.radius + p2.radius;
  sum_radii_sq = sum_radii * sum_radii;
  n[0] = p2.position[0] - p1.position[0];
  n[1] = p2.position[1] - p1.position[1];
  n_mag_sq = n[0] * n[0] + n[1] * n[1];
  if (n_mag_sq <= sum_radii_sq)
    return true;
  else
    return false;
}

void collisionReactionParticles1D(Particle &p1, Particle &p2)
{
  float m1, m2, v1i, v1f, v2i, v2f;

  m1 = p1.mass;
  m2 = p2.mass;
  v1i = p1.velocity[0];
  v2i = p2.velocity[0];
  v1f = (m1 - m2) / (m1 + m2) * v1i + 2.0 * m2 / (m1 + m2) * v2i;
  v2f = 2.0 * m1 / (m1 + m2) * v1i + (m2 - m1) / (m1 + m2) * v2i;

  p1.velocity[0] = v1f;
  p2.velocity[0] = v2f;
}

void collisionReactionParticles2DprojNormal(Particle &p1, Particle &p2)
{
  Real n[2], n_mag;
  Real projnv1, projnv2;
  Real m1, m2, v1i, v2i, v1f, v2f;

  /* Normal vector n between centres. */
  n[0] = p2.position[0] - p1.position[0];
  n[1] = p2.position[1] - p1.position[1];

  /* Normalise n. */
  n_mag = sqrt(n[0] * n[0] + n[1] * n[1]);
  n[0] /= n_mag;
  n[1] /= n_mag;

  /* Vector projection/component/resolute of velocity in n direction. */
  projnv1 = n[0] * p1.velocity[0] + n[1] * p1.velocity[1];
  projnv2 = n[0] * p2.velocity[0] + n[1] * p2.velocity[1];

  /* Use 1D equations to calculate final velocities in n direction. */
  v1i = projnv1;
  v2i = projnv2;
  m1 = p1.mass;
  m2 = p2.mass;
  v1f = (m1 - m2) / (m1 + m2) * v1i + 2.0 * m2 / (m1 + m2) * v2i;
  v2f = 2.0 * m1 / (m1 + m2) * v1i + (m2 - m1) / (m1 + m2) * v2i;

  /* Vector addition to solve for final velocity. */
  p1.velocity[0] = (p1.velocity[0] - v1i * n[0]) + v1f * n[0];
  p1.velocity[1] = (p1.velocity[1] - v1i * n[1]) + v1f * n[1];
  p2.velocity[0] = (p2.velocity[0] - v2i * n[0]) + v2f * n[0];
  p2.velocity[1] = (p2.velocity[1] - v2i * n[1]) + v2f * n[1];
}

void collisionReactionParticles2DbasisChange(Particle &p1, Particle &p2)
{
  Real n[2], t[2], n_mag;
  Real v1_nt[2], v2_nt[2];
  Real m1, m2, v1i, v2i, v1f, v2f;

  if (debug[debug_collisionReactionParticles2DbasisChange]) {
    printf("collisionReactionParticles2DbasisChange:\n");
    printf("velocities before: %f %f %f %f\n",
      p1.velocity[0], p1.velocity[1],
      p2.velocity[0], p2.velocity[1]);
  }

  /* Normal vector n between centres. */
  n[0] = p2.position[0] - p1.position[0];
  n[1] = p2.position[1] - p1.position[1];

  /* Normalise n. */
  n_mag = sqrt(n[0] * n[0] + n[1] * n[1]);
  n[0] /= n_mag;
  n[1] /= n_mag;

  /* Tangent vector t. */
  t[0] = -n[1];
  t[1] = n[0];

  /* Change basis for velocities from standard basis to nt basis. */
  v1_nt[0] = n[0] * p1.velocity[0] + n[1] * p1.velocity[1];
  v1_nt[1] = t[0] * p1.velocity[0] + t[1] * p1.velocity[1];
  v2_nt[0] = n[0] * p2.velocity[0] + n[1] * p2.velocity[1];
  v2_nt[1] = t[0] * p2.velocity[0] + t[1] * p2.velocity[1];

  /* Use 1D equations to calculate final velocities in n direction. */
  m1 = p1.mass;
  m2 = p2.mass;
  v1i = v1_nt[0];
  v2i = v2_nt[0];
  v1f = (m1 - m2) / (m1 + m2) * v1i + 2.0 * m2 / (m1 + m2) * v2i;
  v2f = 2.0 * m1 / (m1 + m2) * v1i + (m2 - m1) / (m1 + m2) * v2i;

  /* Update the 2D velocity. Force is in n direction, so in nt basis,
   * velocity change is in n direction only, no change in t direction.
   */
  v1_nt[0] = v1f;
  v2_nt[0] = v2f;

  /* Change back to standard basis. */
  p1.velocity[0] = n[0] * v1_nt[0] + t[0] * v1_nt[1];
  p1.velocity[1] = n[1] * v1_nt[0] + t[1] * v1_nt[1];
  p2.velocity[0] = n[0] * v2_nt[0] + t[0] * v2_nt[1];
  p2.velocity[1] = n[1] * v2_nt[0] + t[1] * v2_nt[1];

  if (debug[debug_collisionReactionParticles2DbasisChange]) {
    printf("velocities after: %f %f %f %f\n",
      p1.velocity[0], p1.velocity[1],
      p2.velocity[0], p2.velocity[1]);
  }
}

void collideParticlesBruteForce(Real h)
{
  int i, j;

  for (i = 0; i < numParticles - 1; i++) {
    for (j = i + 1; j < numParticles; j++) {
      if (collisionDetectionParticles(particle[i], particle[j])) {

        if (debug[debug_collideParticlesBruteForce])
            printf("collideParticlesBruteForce: collision %d %d\n", i, j);

        /* Take step back. Better approaches possible. */
        eulerStepSingleParticle(particle[i], -h);
        eulerStepSingleParticle(particle[j], -h);

        if (debug[debug_collideParticlesBruteForce]) {
          printf("velocities before: %f %f %f %f\n",
                 particle[i].velocity[0], particle[i].velocity[1],
                 particle[j].velocity[0], particle[j].velocity[1]);
        }

        /* Collision */
      	if (dimension == 1)
      	  collisionReactionParticles1D(particle[i], particle[j]);
      	else if (dimension == 2) {
      	  if (reacCalc == basisChange)
      	    collisionReactionParticles2DbasisChange(particle[i], particle[j]);
      	  else if (reacCalc == projNormal)
      	    collisionReactionParticles2DprojNormal(particle[i], particle[j]);
      	  else
      	    panic("collision reaction calculation not specified\n");
      	}

        if (debug[debug_collideParticlesBruteForce]) {
          printf("velocities after: %f %f %f %f\n",
                 particle[i].velocity[0], particle[i].velocity[1],
                 particle[j].velocity[0], particle[j].velocity[1]);
        }

        /* Step forward. */
        eulerStepSingleParticle(particle[i], h);
        eulerStepSingleParticle(particle[j], h);

        if (debug[debug_collideParticlesBruteForce]) {
          printf("velocities after: %f %f %f %f\n",
                 particle[i].velocity[0], particle[i].velocity[1],
                 particle[j].velocity[0], particle[j].velocity[1]);
        }

        /* Check walls. */
        collideParticleWall(particle[i], arena);
        collideParticleWall(particle[j], arena);

        if (debug[debug_collideParticlesBruteForce]) {
          printf("velocities after: %f %f %f %f\n",
                 particle[i].velocity[0], particle[i].velocity[1],
                 particle[j].velocity[0], particle[j].velocity[1]);
        }
      }
    }
  }
}

inline void calcGridIndex(Particle &p, Arena a,
                   Real gridCellSize[2], int gridNumCells[2],
                   int index[2])
{
  index[0] = (int)((p.position[0] - a.min[0]) / gridCellSize[0]);
  index[1] = (int)((p.position[1] - a.min[1]) / gridCellSize[1]);
  if (debug_range_check) {
    if (index[0] < 0 || index[0] > gridNumCells[0] - 1)
      panic("gridIndex: index out of range\n");
    if (index[1] < 0 || index[1] > gridNumCells[1] - 1)
      panic("gridIndex: index out of range\n");
  }
}

void collideParticlesUniformGrid(Real h)
{
  Real gridCellSize[2];
  int **gridCellParticleCount, **gridCellParticleListEnd, *gridCellParticleList;
  int gridNumCells[2], gridSize, gridIndex[2], gridCellParticleListStart;
  int gridIndexMin[2], gridIndexMax[2];
  int i, j, k, s, t, p1, p2, total;

  /* Work out grid dimensions and allocate. */
  gridNumCells[0] = (int)(sqrt(numParticles) + 1);
  gridNumCells[1] = (int)(sqrt(numParticles) + 1);
  gridCellSize[0] = (arena.max[0] - arena.min[0]) / gridNumCells[0];
  gridCellSize[1] = (arena.max[1] - arena.min[1]) / gridNumCells[1];
  gridSize = gridNumCells[0] * gridNumCells[1];

  /* Assumption. */
  for (i = 0; i < numParticles; i++)
    if (particle[i].radius * 2.0 > gridCellSize[0] ||
        particle[i].radius * 2.0 > gridCellSize[1])
      panic("collideParticlesUniformGrid: particle diameter > cellSize\n");

  /* Allocate arrays. */
  gridCellParticleCount = (int **)malloc(gridNumCells[0] * sizeof(int *));
  if (gridCellParticleCount == 0)
    panic("collideParticlesUniformGrid: malloc failed\n");
  gridCellParticleListEnd = (int **)malloc(gridNumCells[0] * sizeof(int *));
  if (gridCellParticleListEnd == 0)
    panic("collideParticlesUniformGrid: malloc failed\n");
  for (i = 0; i < gridNumCells[0]; i++) {
    gridCellParticleCount[i] = (int *)malloc(gridNumCells[1] * sizeof(int));
    if (gridCellParticleCount[i] == 0)
      panic("collideParticlesUniformGrid: malloc failed\n");
    gridCellParticleListEnd[i] = (int *)malloc(gridNumCells[1] * sizeof(int));
    if (gridCellParticleListEnd[i] == 0)
      panic("collideParticlesUniformGrid: malloc failed\n");
  }
  gridCellParticleList = (int *)malloc(numParticles * sizeof(int));

  /* Initialise grid particle count. */
  for (i = 0; i < gridNumCells[0]; i++)
    for (j = 0; j < gridNumCells[1]; j++)
      gridCellParticleCount[i][j] = 0;

  /* Cell counts. */
  for (i = 0; i < numParticles; i++) {
     calcGridIndex(particle[i], arena, gridCellSize, gridNumCells, gridIndex);
     gridCellParticleCount[gridIndex[0]][gridIndex[1]] += 1;
  }

  if (debug[debug_collideParticlesUniformGrid]) {
    printf("collideParticlesUniformGrid: gridCellParticleCount\n");
    for (i = 0; i < gridNumCells[0]; i++)
      for (j = 0; j < gridNumCells[1]; j++)
        printf("%d %d %d\n", i, j, gridCellParticleCount[i][j]);
  }

  /* Work out end of cell lists by accumulating cell counts. */
  for (i = 0; i < gridNumCells[0]; i++)
    for (j = 0; j < gridNumCells[1]; j++)
      gridCellParticleListEnd[i][j] = 0;

  total = 0;
  for (i = 0; i < gridNumCells[0]; i++)
    for (j = 0; j < gridNumCells[1]; j++) {
      total = total + gridCellParticleCount[i][j];
      gridCellParticleListEnd[i][j] = total - 1;
    }

  if (debug[debug_collideParticlesUniformGrid]) {
    printf("collideParticlesUniformGrid: gridCellParticleListEnd\n");
    for (i = 0; i < gridNumCells[0]; i++)
      for (j = 0; j < gridNumCells[1]; j++)
        printf("%d %d %d\n", i, j, gridCellParticleListEnd[i][j]);
  }

  /* Build particle lists. */
  for (i = 0; i < gridNumCells[0]; i++)
    for (j = 0; j < gridNumCells[1]; j++)
      gridCellParticleCount[i][j] = 0;

  for (i = 0; i < numParticles; i++) {
    calcGridIndex(particle[i], arena, gridCellSize, gridNumCells, gridIndex);
    gridCellParticleList[gridCellParticleListEnd[gridIndex[0]][gridIndex[1]] -
      gridCellParticleCount[gridIndex[0]][gridIndex[1]]] = i;
    gridCellParticleCount[gridIndex[0]][gridIndex[1]] += 1;
  }

  if (debug[debug_collideParticlesUniformGrid]) {
    printf("collideParticlesUniformGrid: gridCellParticleList\n");
    for (i = 0; i < gridNumCells[0]; i++) {
      for (j = 0; j < gridNumCells[1]; j++) {
        gridCellParticleListStart =
          gridCellParticleListEnd[i][j] - gridCellParticleCount[i][j] + 1;
        printf("particle list %d %d\n", i, j);
        for (k = gridCellParticleListStart;
	     k < gridCellParticleListEnd[i][j];
             k++)
          printf("%d\n", gridCellParticleList[k]);
        printf("\n");
      }
    }
  }

  /* Collision detection. */
  for (i = 0; i < numParticles; i++) {
    calcGridIndex(particle[i], arena, gridCellSize, gridNumCells, gridIndex);

    /* Grid index bounds for this particle. */
    gridIndexMin[0] = gridIndex[0] - 1;
    if (gridIndexMin[0] < 0)
      gridIndexMin[0] = 0;
    gridIndexMin[1] = gridIndex[1] - 1;
    if (gridIndexMin[1] < 0)
      gridIndexMin[1] = 0;
    gridIndexMax[0] = gridIndex[0] + 1;
    if (gridIndexMax[0] > gridNumCells[0] - 1)
      gridIndexMax[0] = gridNumCells[0] - 1;
    gridIndexMax[1] = gridIndex[1] + 1;
    if (gridIndexMax[1] > gridNumCells[1] - 1)
      gridIndexMax[1] = gridNumCells[1] - 1;

    p1 = i;

    for (s = gridIndexMin[0]; s <= gridIndexMax[0]; s++) {
      for (t = gridIndexMin[1]; t <= gridIndexMax[1]; t++) {
        gridCellParticleListStart =
          gridCellParticleListEnd[s][t] - gridCellParticleCount[s][t] + 1;
        for (j = gridCellParticleListStart;
             j <= gridCellParticleListEnd[s][t];
             j++) {
          p2 = gridCellParticleList[j];

	  /* Don't test particle against itself. */
          if (p2 == p1)
            continue;

          /* Only test pairs once. */
	  if (p2 < p1)
	    continue;

	  if (debug[debug_collideParticlesUniformGrid])
	    printf("collideParticlesUniformGrid: testing %d %d\n", p1, p2);

	  if (collisionDetectionParticles(particle[p1], particle[p2])) {

	    if (debug[debug_collideParticlesUniformGrid])
              printf("collision: %d %d\n", p1, p2);

	    /* Take step back. Better approaches possible. */
            eulerStepSingleParticle(particle[p1], -h);
            eulerStepSingleParticle(particle[p2], -h);

            if (debug[debug_collideParticlesUniformGrid]) {
              printf("velocities before: %f %f %f %f\n",
                     particle[p1].velocity[0], particle[p1].velocity[1],
                     particle[p2].velocity[0], particle[p2].velocity[1]);
            }

            /* Collision */
	    if (dimension == 1)
	      collisionReactionParticles1D(particle[p1], particle[p2]);
	    else if (dimension == 2) {
	      if (reacCalc == basisChange)
		collisionReactionParticles2DbasisChange(particle[p1], particle[p2]);
	      else if (reacCalc == projNormal)
		collisionReactionParticles2DprojNormal(particle[p1], particle[p2]);
	      else
		panic("collision reaction calculation not specified\n");
	    }

            if (debug[debug_collideParticlesUniformGrid])
              printf("velocities after: %f %f %f %f\n",
                     particle[p1].velocity[0], particle[p1].velocity[1],
                     particle[p1].velocity[0], particle[p2].velocity[1]);

            /* Step forward. */
            eulerStepSingleParticle(particle[p1], h);
            eulerStepSingleParticle(particle[p2], h);

            /* Check walls. */
            collideParticleWall(particle[p1], arena);
            collideParticleWall(particle[p2], arena);
	  }
	}
      }
    }
  }

  /* Free arrays. */
  for (i = 0; i < gridNumCells[0]; i++) {
    free(gridCellParticleCount[i]);
    free(gridCellParticleListEnd[i]);
  }
  free(gridCellParticleCount);
  free(gridCellParticleListEnd);
  free(gridCellParticleList);
}

void updateParticles(void)
{
  static Real time = 0.0, h;

  /* Calculate time increment. */
  h = elapsedTime - time;
  time = elapsedTime;
  if (debug[debug_time])
    printf("updateParticles: time %f %f\n", time, h);

  /* Compute new positions of particles. */
  integrateMotionParticles(h);

  /* Collisions against walls. */
  collideParticlesWall();

  /* Collisions amongst particles. */
  if (CDmethod == bruteForce)
    collideParticlesBruteForce(h);
  else if (CDmethod == uniformGrid)
    collideParticlesUniformGrid(h);
  else
    panic("updateParticles: unknown collision detection method\n");

  if (debug[debug_sum_kinetic_energy]) {
    printf("K = %f\n", sumKineticEnergy());
  }
  if (debug[debug_sum_momentum]) {
    Real p[2];
    sumMomentum(p);
    printf("p = %f %f\n", p[0], p[1]);
  }
}

void displayParticles(void)
{
  int i;

  /* Display particles. */
  for (i = 0; i < numParticles; i++) {
    if (debug[debug_particle])
      printf ("displayParticles: x %f y %f\n",
              particle[i].position[0], particle[i].position[1]);
    glPushMatrix();
    glTranslatef(particle[i].position[0], particle[i].position[1], 0.0);
    displayParticle(&particle[i], 1.0, 1.0, 1.0);
    glPopMatrix();
  }
}

void displayOSD(int frameNo)
{
  static const Real interval = 1.0;
  static Real frameRateInterval = 0.0;
  static int frameNoStartInterval = 0;
  static Real elapsedTimeStartInterval = 0.0;
  static char buffer[80];
  int len, i;

  if (elapsedTime < interval)
    return;

  if (elapsedTime > elapsedTimeStartInterval + interval) {
      frameRateInterval = (frameNo - frameNoStartInterval) /
        (elapsedTime - elapsedTimeStartInterval);
      elapsedTimeStartInterval = elapsedTime;
      frameNoStartInterval = frameNo;
  }

  if (debug[debug_framerate]) {
    printf("displayOSD: frameNo %d elapsedTime %f "
           "frameRateInterval %f\n",
           frameNo, elapsedTime, frameRateInterval);
  }

  sprintf(buffer, "framerate: %5d frametime: %5d",
	  int(frameRateInterval),
	  int(1.0/frameRateInterval*1000));
  glRasterPos2f(-10,-9);
  len = (int)strlen(buffer);
  for (i = 0; i < len; i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, buffer[i]);
}

void update(void)
{
  if (!go)
    return;

  elapsedTime = glutGet(GLUT_ELAPSED_TIME) / (Real)milli - startTime;

  updateParticles();

  glutPostRedisplay();
}

void display(void)
{
  static int frameNo = 0;
  GLenum err;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f (0.8, 0.8, 0.8);

  glPushMatrix();

  /* Display particle and arena. */
  glPushMatrix();
  displayArena();
  displayParticles();
  glPopMatrix();

  /* Display frame rate counter. */
  glPushMatrix();
  displayOSD(frameNo);
  glPopMatrix();
  frameNo++;

  glPopMatrix();

  glutSwapBuffers();
  /* Check for errors. */
  while ((err = glGetError()) != GL_NO_ERROR)
    printf("%s\n",gluErrorString(err));
}

void myInit (void)
{
  setRenderMode(renMode);
  initialiseArena();
  if (initialiseParticles == explicitly)
    initialiseParticlesExplicitly();
  else if (initialiseParticles == randomly)
    initialiseParticlesRandomly();
}

void keyboardCB(unsigned char key, int x, int y)
{
  switch (key) {
  case 'q':
    exit(EXIT_SUCCESS);
    break;
  case 'w':
    changeRenderMode();
    break;
  case 'c':
    debug[debug_range_check] = !debug[debug_range_check];
    break;
  case 'd':
    if (CDmethod == uniformGrid)
      CDmethod = bruteForce;
    else if (CDmethod == bruteForce)
      CDmethod = uniformGrid;
    break;
  case 'r':
    if (reacCalc == projNormal)
      reacCalc = basisChange;
    else if (reacCalc == basisChange)
      reacCalc = projNormal;
    break;
  case 's':
    if (!go) {
      startTime = glutGet(GLUT_ELAPSED_TIME) / (Real)milli;
      go = true;
    }
    break;
  }
  glutPostRedisplay();
}

void myReshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(600, 600);
  glutInitWindowPosition(500, 500);
  glutCreateWindow("Collision Detection and Reaction: Particles in an Arena");
  glutDisplayFunc(display);
  glutIdleFunc(update);
  glutReshapeFunc(myReshape);
  glutKeyboardFunc(keyboardCB);
  myInit();

  glutMainLoop();
}
