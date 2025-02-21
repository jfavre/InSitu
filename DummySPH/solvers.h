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
    struct tipsySph
    {
      T mass;
      T pos[3];
      T vel[3];
      T rho;
      T temp;
    };
    std::vector<tipsySph> scalarsAOS;
    static constexpr int NbofScalarfields = sizeof(tipsySph)/sizeof(T);
#else
    std::vector<T> mass;            // "mass"
    std::vector<T> x, y, z;         // Positions
    std::vector<T> vx, vy, vz;      // Velocities
    std::vector<T> rho;             // Density
    std::vector<T> temp;            // Temperature

    static constexpr int NbofScalarfields = 9;
#endif

    static constexpr T cstMass = 0.12345;
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
    this->rho.resize(this->n);
    this->temp.resize(this->n);
    this->mass.resize(this->n);
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
          this->scalarsAOS[id].pos[0] = bbox_offset*this->par_rank -1.0f + 2.0f*ix/(N - 1.0);
          this->scalarsAOS[id].pos[1] = yy;
          this->scalarsAOS[id].pos[2] = zz;
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
      /* rho is equal to radius_square */
      T R = (this->scalarsAOS[i].pos[0] * this->scalarsAOS[i].pos[0] +
             this->scalarsAOS[i].pos[1] * this->scalarsAOS[i].pos[1] +
             this->scalarsAOS[i].pos[2] * this->scalarsAOS[i].pos[2]);
      this->scalarsAOS[i].rho = R;
      this->scalarsAOS[i].temp = sqrt(R);
      this->scalarsAOS[i].mass = cstMass;
      this->scalarsAOS[i].vel[0] = R;
      this->scalarsAOS[i].vel[1] = R;
      this->scalarsAOS[i].vel[2] = R;
#else
      T R = (this->x[i]*this->x[i] + /* rho is equal to radius_square */
             this->y[i]*this->y[i] +
             this->z[i]*this->z[i]);
      this->rho[i] = R;
      this->temp[i] = sqrt(R);
      this->mass[i] = cstMass;
      this->vx[i] = R;
      this->vy[i] = R;
      this->vz[i] = R;
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
    this->rho.clear();
    this->temp.clear();
    this->mass.clear();
    this->vx.clear();
    this->vy.clear();
    this->vz.clear();
#endif
    };
    
    void simulate_one_timestep()
    {
    this->iteration++;
    this->time = this->iteration * 0.01; // fixed, arbitrary timestep value
    T timeoffset = 0.3*sin((this->iteration * M_PI) / 180.0); // fixed, arbitrary timestep value
    //std::cout << "timeoffset " << timeoffset << std::endl;
    // all variables remain constant over time except "rho" and "temp"
    for (size_t i=0; i < this->n; i++)
      {
#ifdef STRIDED_SCALARS
      T R = ((this->scalarsAOS[i].pos[0]+timeoffset) * (this->scalarsAOS[i].pos[0]+timeoffset) +
             (this->scalarsAOS[i].pos[1]+timeoffset) * (this->scalarsAOS[i].pos[1]+timeoffset) +
             (this->scalarsAOS[i].pos[2]+timeoffset) * (this->scalarsAOS[i].pos[2]+timeoffset));
      this->scalarsAOS[i].rho = R;
      this->scalarsAOS[i].temp = sqrt(R);
      this->scalarsAOS[i].vel[0] = R;
      this->scalarsAOS[i].vel[1] = R;
      this->scalarsAOS[i].vel[2] = R;
#else
      T R = ((this->x[i]+timeoffset)*(this->x[i]+timeoffset) + /* rho is equal to radius_square */
             (this->y[i]+timeoffset)*(this->y[i]+timeoffset) +
             (this->z[i]+timeoffset)*(this->z[i]+timeoffset));
      this->rho[i] = R;
      this->temp[i] = sqrt(R);
      this->vx[i] = R;
      this->vy[i] = R;
      this->vz[i] = R;
#endif
      }

#ifndef STRIDED_SCALARS
    auto minmax = std::minmax_element(this->rho.begin(), this->rho.end());
#endif
    };
};
}
#endif
