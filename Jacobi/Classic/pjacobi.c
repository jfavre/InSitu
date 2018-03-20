/*
 A parallel Jacobi solver for the Laplacian equation in 2D
 Written by Jean M. Favre, Swiss National Supercomputing Center
 Last tested on Sun Dec  3 17:12:09 CET 2017 with Visit 2.13

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef PARALLEL
#include <mpi.h> 
#endif

#include "solvers.h"

#ifdef USE_CATALYST
#include "FEAdaptor.h"
#endif

#ifdef USE_LIBSIM
#include <VisItControlInterface_V2.h>
#include <VisItDataInterface_V2.h>
#include "PJacobi_InSitu.h"
#endif


int main(int argc, char *argv[])
{
  simulation_data sim;
  SimInitialize(&sim);

#ifdef PARALLEL
  sim.cart_dims[0] = sim.cart_dims[1] = 0;
  int PartitioningDimension = 2; // want a 2D MPI partitioning. otherwise set to 1.
  MPI_Init(&argc, &argv);                       /* starts MPI */
  MPI_Comm_rank(MPI_COMM_WORLD, &sim.par_rank); /* get current process id */
  MPI_Comm_size(MPI_COMM_WORLD, &sim.par_size); /* get # procs from env or */

  MPI_Partition(PartitioningDimension, &sim);

  neighbors(&sim);
#endif

// We use (bx + 2) grid points in the X direction, i.e. interior points plus 2 b.c. points
// We use (by + 2) grid points in the Y direction, i.e. interior points plus 2 b.c. points
  // decompose the domain

  AllocateGridMemory(&sim);

  set_initial_bc(&sim);

#ifdef USE_CATALYST
  CatalystInitialize(argc, argv);
  fprintf(stdout,"CatalystInitialize\n");

  while (sim.gdel > TOL)
    {
    simulate_one_timestep(&sim);
    CatalystCoProcess(sim.m,
                      sim.rankx * sim.bx, (1+sim.rankx) * sim.bx+1,
                      sim.ranky * sim.by, (1+sim.ranky) * sim.by+1,
                      0, 0,
                      sim.Temp,
                      sim.iter*0.1, sim.iter,
                      0);
    }
  CatalystCoProcess(sim.m,
                      sim.rankx * sim.bx, (1+sim.rankx) * sim.bx+1,
                      sim.ranky * sim.by, (1+sim.ranky) * sim.by+1,
                      0, 0,
                      sim.Temp,
                      sim.iter*0.1, sim.iter,
                      1);

#elif USE_LIBSIM
  LibSimInitialize(&sim, argc, argv);
  fprintf(stdout,"LibSim Initialize\n");

  if(sim.batch)
    mainloop_batch(&sim);
  else
    mainloop_interactive(&sim);

#else
  while ((sim.gdel > TOL) && (sim.iter <= MAXSTEPS))
    {  // iterate until error below threshold
    simulate_one_timestep(&sim);
    }
#endif

#ifdef PARALLEL
  if (!sim.par_rank)
#endif
    fprintf(stdout,"Stopped at iteration %d\nThe maximum error = %f\n", sim.iter, sim.gdel);

  WriteFinalGrid(&sim);

#ifdef USE_CATALYST
  CatalystFinalize();
#endif

  FreeGridMemory(&sim);

#ifdef PARALLEL
  MPI_Finalize();
#endif

  return (0);
}

