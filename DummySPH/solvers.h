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
    T                   time;
    size_t              n;               // Number of particles


#ifdef STRIDED_SCALARS
    struct all_scalars_fields
    {
      T x;
      T y;
      T z;
      T density;
      T pressure;
      T cstfield;
      T vx;
      T vy;
      T vz;
    };
    std::vector<all_scalars_fields> scalarsAOS;
    static constexpr int NbofScalarfields = sizeof(all_scalars_fields)/sizeof(T);
#else
    std::vector<T> x, y, z;         // Positions
    std::vector<T> vx, vy, vz;      // Velocities
    std::vector<T> density;         // Density
    std::vector<T> pressure;        // Pressure
    std::vector<T> cstfield;        // "cst-field"
    static constexpr int NbofScalarfields = 9;
#endif

    static constexpr T value = 0.12345;
    static constexpr T bbox_offset = 2.0; // to offset each MPI partition in 3D space
    
    void AllocateGridMemory(int N)
    {
    MPI_Comm_rank(MPI_COMM_WORLD, &this->par_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &this->par_size);
    this->n = N*N*N;
    this->iteration = 0;

#ifdef STRIDED_SCALARS
    this->scalarsAOS.resize(this->n);
#else
    this->x.resize(this->n);
    this->y.resize(this->n);
    this->z.resize(this->n);
    this->density.resize(this->n);
    this->pressure.resize(this->n);
    this->cstfield.resize(this->n);
    this->vx.resize(this->n);
    this->vy.resize(this->n);
    this->vz.resize(this->n);
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
#ifdef STRIDED_SCALARS
          this->scalarsAOS[id].x = bbox_offset*this->par_rank -1.0f + 2.0f*ix/(N - 1.0);
          this->scalarsAOS[id].y = yy;
          this->scalarsAOS[id].z = zz;
#else
          this->x[id] = bbox_offset*this->par_rank -1.0f + 2.0f*ix/(N - 1.0);
          this->y[id] = yy;
          this->z[id] = zz;
#endif
          id++;
         }
      }
    }

    for (size_t i=0; i < this->n; i++)
      {
#ifdef STRIDED_SCALARS
      T R = (this->scalarsAOS[i].x*this->scalarsAOS[i].x + /* rho is equal to radius_square */
             this->scalarsAOS[i].y*this->scalarsAOS[i].y +
             this->scalarsAOS[i].z*this->scalarsAOS[i].z);
      this->scalarsAOS[i].density = R;
      this->scalarsAOS[i].pressure = sqrt(R);
      this->scalarsAOS[i].cstfield = value;
      this->scalarsAOS[i].vx = R;
      this->scalarsAOS[i].vy = R;
      this->scalarsAOS[i].vz = rand()/(float)RAND_MAX;
#else
      T R = (this->x[i]*this->x[i] + /* rho is equal to radius_square */
             this->y[i]*this->y[i] +
             this->z[i]*this->z[i]);
      this->density[i] = R;
      this->pressure[i] = sqrt(R);
      this->cstfield[i] = value;
      this->vx[i] = R;
      this->vy[i] = rand()/(float)RAND_MAX;
      this->vz[i] = rand()/(float)RAND_MAX;
#endif
      }
    //std::cout << "Vectors of size " << sim.x.size() << std::endl;
    };

    void FreeGridMemory()
    {
#ifdef STRIDED_SCALARS
    this->scalarsAOS.clear();
#else
    this->x.clear();
    this->y.clear();
    this->z.clear();
    this->density.clear();
    this->pressure.clear();
    this->cstfield.clear();
    this->vx.clear();
    this->vy.clear();
    this->vz.clear();
#endif
    };
    
    void simulate_one_timestep()
    {
    this->iteration++;
    this->time = this->iteration * 0.01; // fixed, arbitrary timestep value
    for (size_t i=0; i < this->n; i++)
      {
#ifdef STRIDED_SCALARS
      T R = (this->scalarsAOS[i].x*this->scalarsAOS[i].x + /* rho is equal to radius_square */
             this->scalarsAOS[i].y*this->scalarsAOS[i].y +
             this->scalarsAOS[i].z*this->scalarsAOS[i].z);
      this->scalarsAOS[i].density = R;
      this->scalarsAOS[i].pressure = sqrt(R);
      this->scalarsAOS[i].cstfield = value;
#else
      T R = ((this->x[i]+this->time)*(this->x[i]+this->time) + /* rho is equal to radius_square */
             (this->y[i]+this->time)*(this->y[i]+this->time) +
             (this->z[i]+this->time)*(this->z[i]+this->time));
      this->density[i] = R;
      this->pressure[i] = sqrt(R);
#endif
      }

#ifndef STRIDED_SCALARS
    auto minmax = std::minmax_element(this->density.begin(), this->density.end());
#endif
    };
};
}
#endif
