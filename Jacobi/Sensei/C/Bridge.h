#ifndef PJACOBI_BRIDGE_H
#define PJACOBI_BRIDGE_H

#include <mpi.h>
#include <stdint.h>
#include <solvers.h>

#ifdef __cplusplus
extern "C" {
#endif
  /// This defines the analysis bridge for the pjacobi miniapp.

  /// Called before simulation loop
  void bridge_initialize(MPI_Comm comm, simulation_data *sim, const char *config);

  /// Called per timestep in the simulation loop
  void bridge_update(simulation_data *sim);

  /// Called just before simulation terminates.
  void bridge_finalize(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
