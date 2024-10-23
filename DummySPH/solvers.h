#ifndef _SOLVERS_H_INCLUDED_
#define _SOLVERS_H_INCLUDED_

#include <vector>

namespace sph
{

struct ParticlesData
{
    int                 par_rank;
    int                 par_size;
    int                 iteration;       // Current iteration
    double              time;
    size_t              n;               // Number of particles
    float               angle;
    std::vector<double> x, y, z;         // Positions
    std::vector<double> vx, vy, vz;      // Velocities
#ifdef STRIDED_SCALARS
    std::vector<double> scalars;         // NbofScalarfields interleaved scalars
#else
    std::vector<double> scalar1;         // density
    std::vector<double> scalar2;         // mass
    std::vector<double> scalar3;         // pressure
#endif
    int NbofScalarfields = 3;
} ;

void AllocateGridMemory(ParticlesData &sim, int N);

void FreeGridMemory(ParticlesData &sim);

void simulate_one_timestep(ParticlesData &sim);
}
#endif
