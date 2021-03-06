#ifndef _SOLVERS_H_INCLUDED_
#define _SOLVERS_H_INCLUDED_

typedef struct
{
    int       par_rank;
    int       par_size;
#ifdef PARALLEL
    MPI_Comm  topocomm;
    int       east, west, south, north;
    int       cart_dims[2], rankx, ranky;
#endif
    int       bx, by;
    int       m; // overall grid size not counting boundary walls.
    int      iter;
    int      runMode;
    double   gdel, *oldTemp, *Temp;
    float    *cx, *cy;
    int      savingFiles;
    int      saveCounter;
    int      batch;
    int      export;
    char     *sessionfile;
} simulation_data;

#define BASENAME "."
//#define BASENAME "/scratch/leone/jfavre"
//#define BASENAME "/scratch/snx3000/jfavre"

#define INCREMENT 10       /* number of steps between convergence check  */
#define MAXSTEPS 10000     /* Maximum number of iterations               */
#define TOL 1e-04          /* Numerical Tolerance */

void SimInitialize(simulation_data *sim);
void MPI_Partition(int PartitioningDimension, simulation_data *sim);
void AllocateGridMemory(simulation_data *sim);
void FreeGridMemory(simulation_data *sim);
void set_initial_bc(simulation_data *sim);
double update_jacobi(simulation_data *sim);
void simulate_one_timestep(simulation_data *sim);
void CopyTempValues_2_OldValues(simulation_data *sim);
void WriteFinalGrid(simulation_data *sim);
#ifdef PARALLEL
void exchange_ghost_lines(simulation_data *sim);
void neighbors(simulation_data *sim);
void MPIIOWriteData(const char *filename, simulation_data *sim);
extern MPI_Datatype rowtype, coltype;
#endif

#define SIM_STOPPED       0
#define SIM_RUNNING       1
#endif
