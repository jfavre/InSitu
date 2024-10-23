
#include <mpi.h> 

#include "solvers.h"

#ifdef USE_CATALYST
#include "CatalystAdaptor.h"
#endif

#ifdef USE_ASCENT
#include <ascent/ascent.hpp>
#include "AscentAdaptor.h"
#endif

int main(int argc, char *argv[])
{
  int it=0, Niterations = 10, Nparticles = 500;
  MPI_Init(&argc, &argv);

  sph::ParticlesData sim;
  sph::AllocateGridMemory(sim, Nparticles);

#ifdef USE_CATALYST
  CatalystAdaptor::Initialize(argc, argv);
  std::cout << "Catalyst::Initialize" << std::endl;
#endif
#ifdef USE_ASCENT
  //std::cout << ascent::about() << std::endl;
  AscentAdaptor::Initialize(argc, argv, sim);
  std::cout << "Ascent::Initialize" << std::endl;
#endif

  while (it < Niterations)
    {
    sph::simulate_one_timestep(sim);

#ifdef USE_CATALYST
    CatalystAdaptor::Execute(sim);
#endif
#ifdef USE_ASCENT
  AscentAdaptor::Execute(sim);
  //std::cout << "Ascent::Execute" << std::endl;
#endif
    it++;
    }

#ifdef USE_CATALYST
  CatalystAdaptor::Finalize();
  std::cout << "Catalyst::Finalize" << std::endl;
#endif
#ifdef USE_ASCENT
  AscentAdaptor::Finalize();
  std::cout << "Ascent::Finalize" << std::endl;
#endif

  sph::FreeGridMemory(sim);

  MPI_Finalize();

  return (0);
}

