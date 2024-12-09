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
  int it = 0, Niterations = 100, Nparticles = 25;
  int frequency = 10;
  MPI_Init(&argc, &argv);
    
  ParticlesData<float> *sim = new(ParticlesData<float>);
  sim->AllocateGridMemory(Nparticles);

  viz::init_catalyst(argc, argv);
  viz::init_ascent(sim);
  viz::init_vtkm(argc, argv, sim);

  while (it < Niterations)
    {
    sim->simulate_one_timestep();
    viz::execute(sim, it, frequency);
    it++;
    }

  viz::finalize();

  sim->FreeGridMemory();

  MPI_Finalize();

  return (0);
}

