/*
We currently have 3 different APIs for in-situ visualization

0) Catalyst
cd buildCatalyst2
make && ./bin/dummysph_catalystV2  --pv --catalyst ../catalyst_state.py

1) Ascent
cd buildAscent
make && ./bin/dummysph_ascent (# reads the ascent_actions.yaml present in current directory)

2) VTK-m
cd buildVTKm
make && ./bin/dummysph_vtkm


Written by Jean M. Favre, Swiss National Supercomputing Center
Tested Mon  9 Dec 13:50:37 CET 2024
*/

#include <mpi.h> 
#include <sstream>

#include "insitu_viz.h"

using namespace sph;

int main(int argc, char *argv[])
{
  int it = 0, Niterations = 5, Nparticles = 500; // actually Nparticles^3
  int frequency = 5;
  int rank = 0;
  const bool quiet = false;
  std::ofstream nullOutput("/dev/null");
  std::ostream& output = (quiet || rank) ? nullOutput : std::cout;
  sphexa::Timer timer(output);
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
  ParticlesData<double> *sim = new(ParticlesData<double>);
  sim->AllocateGridMemory(Nparticles);

  viz::init(argc, argv, sim);

  timer.start();
  timer.step("post-init");
  while (it < Niterations)
    {
    sim->simulate_one_timestep();
    it++;
    viz::execute(sim, it, frequency);
    }
  timer.step("post-exec");

  viz::finalize(sim);

  sim->FreeGridMemory();

  MPI_Finalize();

  return (0);
}

