#ifndef _SOLVERS_H_INCLUDED_
#define _SOLVERS_H_INCLUDED_

#include <vector>
#include <math.h>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <mpi.h>

namespace sph
{
template<typename T>
class ParticlesData
{
  public:
    int                 par_rank;
    int                 par_size;
    int                 iteration;       // Current iteration
    double              time;
    size_t              n;               // Number of particles
    std::vector<T> x, y, z;         // Positions
    std::vector<T> vx, vy, vz;      // Velocities
#ifdef STRIDED_SCALARS
    struct uvw
    {
      T density;
      T pressure;
      T cstfield;
    };
    std::vector<uvw> scalarsAOS;
#else
    std::vector<T> scalar1;         // Density
    std::vector<T> scalar2;         // Pressure
    std::vector<T> scalar3;         // "cst-field"
#endif
    static constexpr int NbofScalarfields = 3;
    static constexpr T value = 0.12345;
    static constexpr T bbox_offset = 2.0; // to offset each MPI partition in 3D space
    
    void AllocateGridMemory(int N)
    {
    MPI_Comm_rank(MPI_COMM_WORLD, &this->par_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &this->par_size);
    this->n = N*N*N;
    this->iteration = 0;
    this->x.resize(this->n);
    this->y.resize(this->n);
    this->z.resize(this->n);
    this->vx.resize(this->n);
    this->vy.resize(this->n);
    this->vz.resize(this->n);
#ifdef STRIDED_SCALARS
    this->scalarsAOS.resize(this->n);
#else
    this->scalar1.resize(this->n);
    this->scalar2.resize(this->n);
    this->scalar3.resize(this->n);
#endif
    srand(1234 + this->par_rank);
    //std::cerr << "adding offset for coordinates " << sim.par_rank << std::endl;
    int id=0;
    for (auto iz=0; iz < N; iz++) {
      T zz = -1.0f + 2.0f*iz/(N - 1.0f);
      for (auto iy=0; iy < N; iy++){
        T yy = bbox_offset*this->par_rank - 1.0f + 2.0f*iy/(N - 1.0f);
        for (auto ix=0; ix < N; ix++)
          {
          this->x[id] = bbox_offset*this->par_rank -1.0f + 2.0f*ix/(N - 1.0);
          this->y[id] = yy;
          this->z[id] = zz;
          id++;
         }
      }
    }

    for (size_t i=0; i < this->n; i++)
      {
      T R = (this->x[i]*this->x[i] + /* rho is equal to radius_square */
             this->y[i]*this->y[i] +
             this->z[i]*this->z[i]);
#ifdef STRIDED_SCALARS
      this->scalarsAOS[i].density = R;
      this->scalarsAOS[i].pressure = sqrt(R);
      this->scalarsAOS[i].cstfield = value;
#else
      this->scalar1[i] = R;
      this->scalar2[i] = sqrt(R);
      this->scalar3[i] = value;
#endif
      this->vx[i] = R;
      this->vy[i] = rand()/(float)RAND_MAX;
      this->vz[i] = rand()/(float)RAND_MAX;
      }
    //std::cout << "Vectors of size " << sim.x.size() << std::endl;
    };

    void FreeGridMemory()
    {
    this->x.clear();
    this->y.clear();
    this->z.clear();
    this->vx.clear();
    this->vy.clear();
    this->vz.clear();
#ifdef STRIDED_SCALARS
    this->scalarsAOS.clear();
#else
    this->scalar1.clear();
    this->scalar2.clear();
    this->scalar3.clear();
#endif
    };
    
    void simulate_one_timestep()
    {
    this->iteration++;
    this->time = this->iteration * 0.01; // fixed, arbitrary timestep value
    for (size_t i=0; i < this->n; i++)
      {
      T R = ((this->x[i]+this->time)*(this->x[i]+this->time) + /* rho is equal to radius_square */
             (this->y[i]+this->time)*(this->y[i]+this->time) +
             (this->z[i]+this->time)*(this->z[i]+this->time));
#ifdef STRIDED_SCALARS
      this->scalarsAOS[i].density = R;
      this->scalarsAOS[i].pressure = sqrt(R);
      this->scalarsAOS[i].cstfield = value;
#else
      this->scalar1[i] = R;
      this->scalar2[i] = sqrt(R);
#endif
      }

#ifndef STRIDED_SCALARS
    auto minmax = std::minmax_element(this->scalar1.begin(), this->scalar1.end());
#endif
    };
};
}
#endif
