#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <mpi.h>
#include "solvers.h"

namespace sph
{
void AllocateGridMemory(ParticlesData &sim, int N)
{
  MPI_Comm_rank(MPI_COMM_WORLD, &sim.par_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &sim.par_size);
  sim.n = N;
  sim.iteration = 0;
  sim.angle = 0.0;
  sim.x.resize(N);
  sim.y.resize(N);
  sim.z.resize(N);
  sim.vx.resize(N);
  sim.vy.resize(N);
  sim.vz.resize(N);
#ifdef STRIDED_SCALARS
  sim.scalars.resize(sim.NbofScalarfields * sim.n);
#else
  sim.scalar1.resize(N);
  sim.scalar2.resize(N);
  sim.scalar3.resize(N);
#endif
  srand(1234+sim.par_rank);

  for (auto i=0; i < N; i++)
    {
    float t = ((float)i) / ((float)(N-1));
    float a = 3.14159 * .05 * t;
    sim.x[i] = 1 * cos(a + (0.5 + 0.5 * t) * sim.angle);
    sim.y[i] = 1 * sin(a + (0.5 + 0.5 * t) * sim.angle);
    sim.z[i] = t;
#ifdef STRIDED_SCALARS
    sim.scalars[sim.NbofScalarfields*i] = sqrt(sim.x[i]*sim.x[i] + /* rho is equal to radius */
                                               sim.y[i]*sim.y[i] +
                                               sim.z[i]*sim.z[i]);
    sim.scalars[sim.NbofScalarfields*i + 1] = -10000;
    sim.scalars[sim.NbofScalarfields*i + 2] = 10000;
#else
    sim.scalar1[i] = sqrt(sim.x[i]*sim.x[i] + /* rho is equal to radius */
                          sim.y[i]*sim.y[i] +
                          sim.z[i]*sim.z[i]);
    sim.scalar2[i] = -10000 +i;
    sim.scalar3[i] = 10000 + i;
#endif
    sim.vx[i] = rand()/(float)RAND_MAX;
    sim.vy[i] = rand()/(float)RAND_MAX;
    sim.vz[i] = rand()/(float)RAND_MAX;
    }
  //std::cout << "Vectors of size " << sim.x.size() << std::endl;
} ;

void FreeGridMemory(ParticlesData &sim)
{
};

void simulate_one_timestep(ParticlesData &sim)
{
  sim.iteration++;
  sim.time = sim.iteration * 0.01; // fixed, arbitrary timestep value
  for (auto i=0; i < sim.n; i++)
    {
    float t = ((float)i) / ((float)(sim.n-1));
    float a = 3.14159 * 1. * t;
    sim.x[i] = t*cos(a + (0.5 + 0.5 * t) * sim.angle);
    sim.y[i] = t*sin(a + (0.5 + 0.5 * t) * sim.angle);
    sim.z[i] = t;
#ifdef STRIDED_SCALARS
    sim.scalars[sim.NbofScalarfields*i] = 2.0*3.141592*(sqrt(sim.x[i]*sim.x[i] +
                                                             sim.y[i]*sim.y[i] +
                                                             sim.z[i]*sim.z[i]) - 0.5);
    sim.scalars[sim.NbofScalarfields*i + 1] = -10000;
    sim.scalars[sim.NbofScalarfields*i + 2] = 10000;
    
#else
    sim.scalar1[i] = 2.0*3.141592*(sqrt(sim.x[i]*sim.x[i] +
                     sim.y[i]*sim.y[i] +
                     sim.z[i]*sim.z[i]) - 0.5);
#endif
    }
  sim.angle = sim.angle + 0.015*30;
#ifdef STRIDED_SCALARS
  auto minmax = std::minmax_element(sim.scalars.begin(), sim.scalars.end());
#else
  auto minmax = std::minmax_element(sim.scalar1.begin(), sim.scalar1.end());
#endif
  //std::cerr << sim.time << ": " << *minmax.first << " << density << " << *minmax.second << std::endl;

};
}
