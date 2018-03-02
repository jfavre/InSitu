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
    int g_x, int g_y, int g_z,
    int l_x, int l_y, int l_z,
    uint64_t start_extents_x, uint64_t start_extents_y, uint64_t start_extents_z,
    const char* config_file);

  /// Called per timestep in the simulation loop
  void bridge_update(int tstep, double time, double* temperature);

  /// Called just before simulation terminates.
  void bridge_finalize();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
