#ifndef _SOLVERS_H_INCLUDED_
#define _SOLVERS_H_INCLUDED_

#include <vector>

namespace sph
{

struct ParticlesData
{
    int       par_rank;
    int       par_size;
    int      iteration;                      // Current iteration
    double    time;
    size_t    n;                              // Number of particles
    float angle;
    std::vector<double> x, y, z;              // Positions
    std::vector<double> vx, vy, vz;           // Velocities
    std::vector<double> ro;                   // density
} ;

void AllocateGridMemory(ParticlesData &sim, int N);

void FreeGridMemory(ParticlesData &sim);

void simulate_one_timestep(ParticlesData &sim);
}
#endif
