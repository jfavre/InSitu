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
  sim.ro.resize(N);
  srand(1234+sim.par_rank);

  for (auto i=0; i < N; i++)
    {
    float t = ((float)i) / ((float)(N-1));
    float a = 3.14159 * 10. * t;
    sim.x[i] = t * cos(a + (0.5 + 0.5 * t) * sim.angle);
    sim.y[i] = t * sin(a + (0.5 + 0.5 * t) * sim.angle);
    sim.z[i] = t;
    sim.ro[i] = sqrt(sim.x[i]*sim.x[i] +
                     sim.y[i]*sim.y[i] +
                     sim.z[i]*sim.z[i]);
    
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
    float a = 3.14159 * 10. * t;
    sim.x[i] = t * cos(a + (0.5 + 0.5 * t) * sim.angle);
    sim.y[i] = t * sin(a + (0.5 + 0.5 * t) * sim.angle);
    sim.z[i] = t;
    sim.ro[i] = sqrt(sim.x[i]*sim.x[i] +
                     sim.y[i]*sim.y[i] +
                     sim.z[i]*sim.z[i]);
    }
  sim.angle = sim.angle + 0.05;
  auto minmax = std::minmax_element(sim.ro.begin(), sim.ro.end());
  std::cerr << *minmax.first << " << density << " << *minmax.second << std::endl;
};
}
