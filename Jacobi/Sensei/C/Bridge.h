#ifndef PJACOBI_BRIDGE_H
#define PJACOBI_BRIDGE_H

#include <mpi.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
  /// This defines the analysis bridge for the pjacobi miniapp.

  /// Called before simulation loop
  void bridge_initialize(MPI_Comm comm,
    int m, int rankx, int ranky, int bx, int by, int ng,
    const char* config_file);

  /// Called per timestep in the simulation loop
  void bridge_update(int tstep, double time, double* temperature);

  /// Called just before simulation terminates.
  void bridge_finalize();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
