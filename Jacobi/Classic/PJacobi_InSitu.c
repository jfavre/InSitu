#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef PARALLEL
#include <mpi.h>
#endif
#include "solvers.h"

#include <VisItControlInterface_V2.h>
#include <VisItDataInterface_V2.h>
#include <PJacobi_InSitu.h>

#define VISIT_COMMAND_PROCESS 0
#define VISIT_COMMAND_SUCCESS 1
#define VISIT_COMMAND_FAILURE 2

#ifdef PARALLEL
int visit_broadcast_int_callback(int *value, int sender, void *cbdata)
{
  simulation_data *sim = (simulation_data *)cbdata;
  return MPI_Bcast(value, 1, MPI_INT, sender, sim->topocomm);
}

int visit_broadcast_string_callback(char *str, int len, int sender, void *cbdata)
{
  simulation_data *sim = (simulation_data *)cbdata;
  return MPI_Bcast(str, len, MPI_CHAR, sender, sim->topocomm);
}
#endif

void mainloop_interactive(simulation_data *sim)
{
  int blocking, visitstate, err = 0;
  while ((sim->gdel > TOL) && (sim->iter <= MAXSTEPS))
    {
    blocking = (sim->runMode == SIM_RUNNING) ? 0 : 1;

        /* Get input from VisIt or timeout so the simulation can run. */
    if(sim->par_rank == 0)
      visitstate = VisItDetectInput(blocking, -1);
#ifdef PARALLEL
    MPI_Bcast(&visitstate, 1, MPI_INT, 0, sim->topocomm);
#endif
        /* Do different things depending on the output from VisItDetectInput. */
    switch(visitstate)
      {
      case 0:
        /* There was no input from VisIt, return control to sim. */
        simulate_one_timestep(sim);
        Create_InSitu_Outputs(sim);
      break;
      case 1:
            /* VisIt is trying to connect to sim. */
        if(VisItAttemptToCompleteConnection() == VISIT_OKAY)
          {
          if(1) //!sim.par_rank)
            {
            fprintf(stderr, "VisIt connected\n");
            }
          SetupCallbacks(sim);
          }
        else 
          {
           /* Print the error message */
          char *err = VisItGetLastError();
          fprintf(stderr, "VisIt did not connect: %s\n", err);
          free(err);
          }
      break;
      case 2:
        /* VisIt wants to tell the engine something. */
        if(!ProcessVisItCommand(sim))
          {
          /* Disconnect on an error or closed connection. */
          VisItDisconnect();
          if(!sim->par_rank)
            {
            fprintf(stderr, "VisIt disconnected\n");
            }
          // Start running again if VisIt closes. 
          //runMode = SIM_RUNNING;
          }
      break;
      default:
        fprintf(stderr, "Can't recover from error %d!\n", visitstate);
      break;
      }
    }
}

void mainloop_batch(simulation_data *sim)
{
    /* Explicitly load VisIt runtime functions and install callbacks. */
    VisItInitializeRuntime();
    SetupCallbacks(sim);

    /* Set up some plots. */
    simulate_one_timestep(sim);
    Create_InSitu_Outputs(sim);

    if(sim->sessionfile != NULL)
    {
        if(VisItRestoreSession(sim->sessionfile) != 0)
        {
            if(sim->par_rank == 0)
            {
                fprintf(stderr, "Could not restore session file %s\n",
                        sim->sessionfile);
            }
            return;
        }
    }
    else
    {
        /* Set up some plots using libsim functions. */
        //VisItAddPlot("Mesh", "mesh");
        VisItAddPlot("Contour", "temperature");
        VisItAddPlot("Pseudocolor", "temperature");
        VisItDrawPlots();
    }

    /* Turn on image saving. */
    sim->savingFiles = 1;

    /* Iterate over time. */
    while (sim->gdel > TOL)
      {
      simulate_one_timestep(sim);
      Create_InSitu_Outputs(sim);
      }
}

void LibSimInitialize(simulation_data *sim, int argc, char *argv[])
{
  char *env = NULL;

  for(int i = 1; i < argc; ++i)
    {
    if(strcmp(argv[i], "-batch") == 0)
      sim->batch = 1;
    else if(strcmp(argv[i], "-export") == 0)
      sim->export = 1;
    else if(strcmp(argv[i], "-sessionfile") == 0 && (i+1) < argc)
      {
      sim->sessionfile = strdup(argv[i+1]);
      ++i;
      }
    }

#ifdef PARALLEL
  // use only to debug and write trace files for each rank
  //char name[128];
  //sprintf(name, BASENAME"/jacobi.%03d.txt", sim->par_rank);
  //fprintf(stderr, "name = %s\n", name);
  //VisItOpenTraceFile(name);
  VisItSetBroadcastIntFunction2(visit_broadcast_int_callback, (void*)sim);
  VisItSetBroadcastStringFunction2(visit_broadcast_string_callback, (void*)sim);
  VisItSetParallel(sim->par_size > 1);
  VisItSetParallelRank(sim->par_rank);
  VisItSetMPICommunicator((void *)&sim->topocomm);
#endif

  if(sim->par_rank == 0)
     env = VisItGetEnvironment();

    /* Pass the environment to all other processors collectively. */
  VisItSetupEnvironment2(env);
  if(env != NULL)
    free(env);

  if(!sim->par_rank)
    {
    VisItInitializeSocketAndDumpSimFile(
#ifdef PARALLEL
            "pjacobi",
#else
            "jacobi",
#endif
            "Jacobi solver for Laplace Equation",
            "/path/to/where/sim/was/started",
            NULL, NULL, "pjacobi.sim2");
    }
}

const char *cmd_names[] = {"halt", "step", "run", "update", "restart"};

visit_handle
SimGetMetaData(void *cbdata)
{
  simulation_data *sim = (simulation_data *)cbdata;
  visit_handle md = VISIT_INVALID_HANDLE;

  if(VisIt_SimulationMetaData_alloc(&md) == VISIT_OKAY)
    {
    int i;
    double simtime = sim->iter * 0.1;
    visit_handle m1 = VISIT_INVALID_HANDLE;
    visit_handle vmd = VISIT_INVALID_HANDLE;
    visit_handle cmd = VISIT_INVALID_HANDLE;

        /* Set the simulation state. */
    VisIt_SimulationMetaData_setMode(md, (sim->runMode == SIM_STOPPED) ?
                VISIT_SIMMODE_STOPPED : VISIT_SIMMODE_RUNNING);
    VisIt_SimulationMetaData_setCycleTime(md, sim->iter, simtime);

        /* Set the first mesh's properties.*/
    if(VisIt_MeshMetaData_alloc(&m1) == VISIT_OKAY)
      {
      /* Set the mesh's properties.*/
      VisIt_MeshMetaData_setName(m1, "mesh");
      VisIt_MeshMetaData_setMeshType(m1, VISIT_MESHTYPE_RECTILINEAR);
      VisIt_MeshMetaData_setTopologicalDimension(m1, 2);
      VisIt_MeshMetaData_setSpatialDimension(m1, 2);
      VisIt_MeshMetaData_setNumDomains(m1, sim->par_size);
      VisIt_MeshMetaData_setDomainTitle(m1, "Domains");
      VisIt_MeshMetaData_setDomainPieceName(m1, "domain");
      VisIt_MeshMetaData_setNumGroups(m1, 0);
      VisIt_MeshMetaData_setXUnits(m1, "cm");
      VisIt_MeshMetaData_setYUnits(m1, "cm");
      VisIt_MeshMetaData_setXLabel(m1, "Width");
      VisIt_MeshMetaData_setYLabel(m1, "Height");
    
      VisIt_SimulationMetaData_addMesh(md, m1);
      }

      /* Add a nonal scalar variable on mesh. */
    if(VisIt_VariableMetaData_alloc(&vmd) == VISIT_OKAY)
      {
      VisIt_VariableMetaData_setName(vmd, "temperature");
      VisIt_VariableMetaData_setMeshName(vmd, "mesh");
      VisIt_VariableMetaData_setType(vmd, VISIT_VARTYPE_SCALAR);
      VisIt_VariableMetaData_setCentering(vmd, VISIT_VARCENTERING_NODE);
      VisIt_SimulationMetaData_addVariable(md, vmd);
      }

      /* Add some custom commands. */
    for(i = 0; i < sizeof(cmd_names)/sizeof(const char *); ++i)
      {
      visit_handle cmd = VISIT_INVALID_HANDLE;
      if(VisIt_CommandMetaData_alloc(&cmd) == VISIT_OKAY)
        {
        VisIt_CommandMetaData_setName(cmd, cmd_names[i]);
        VisIt_SimulationMetaData_addGenericCommand(md, cmd);
        }
      }
    }

  return md;
}

visit_handle
SimGetMesh(int domain, const char *name, void *cbdata)
{
  visit_handle h = VISIT_INVALID_HANDLE;
  simulation_data *sim = (simulation_data *)cbdata;
  if(strcmp(name, "mesh") == 0)
    {
    if(VisIt_RectilinearMesh_alloc(&h) != VISIT_ERROR)
      {
      int minRealIndex[3], maxRealIndex[3];
      minRealIndex[0] = 0;// default case for a domain not touching the walls
      minRealIndex[1] = 0;// default case for a domain not touching the walls
      minRealIndex[2] = 0;  
      maxRealIndex[0] = sim->bx;// default case for a domain not touching the walls
      maxRealIndex[1] = sim->by;// default case for a domain not touching the walls
      maxRealIndex[2] = 0;
#ifdef PARALLEL
      if(sim->rankx == (sim->cart_dims[0]-1))
        maxRealIndex[0]++;
      if(sim->ranky == (sim->cart_dims[1]-1))
        maxRealIndex[1]++;
#endif
      visit_handle hxc, hyc;
      VisIt_VariableData_alloc(&hxc);
      VisIt_VariableData_alloc(&hyc);
      VisIt_VariableData_setDataF(hxc, VISIT_OWNER_SIM, 1, sim->bx+2, sim->cx);
      VisIt_VariableData_setDataF(hyc, VISIT_OWNER_SIM, 1, sim->by+2, sim->cy);
      VisIt_RectilinearMesh_setCoordsXY(h, hxc, hyc);

      VisIt_RectilinearMesh_setRealIndices(h, minRealIndex, maxRealIndex);
      }
    }
  return h;
}

visit_handle
SimGetVariable(int domain, const char *name, void *cbdata)
{
  visit_handle h = VISIT_INVALID_HANDLE;
  simulation_data *sim = (simulation_data *)cbdata;
  int nComponents = 1, nTuples;

  if(VisIt_VariableData_alloc(&h) == VISIT_OKAY)
    {
    if(strcmp(name, "temperature") == 0)
      {
      nTuples = (sim->bx+2)*(sim->by+2);
      VisIt_VariableData_setDataD(h, VISIT_OWNER_SIM, nComponents, nTuples, sim->Temp);
      }
    else
      {
      VisIt_VariableData_free(h);
      h = VISIT_INVALID_HANDLE;
      }
    }
  return h;
}

visit_handle
SimGetDomainList(const char *name, void *cbdata)
{
  visit_handle h = VISIT_INVALID_HANDLE;
  if(VisIt_DomainList_alloc(&h) != VISIT_ERROR)
    {
    visit_handle hdl;
    int i, *iptr = NULL;
    simulation_data *sim = (simulation_data *)cbdata;
    iptr = (int *)malloc(sizeof(int));
    *iptr = sim->par_rank;

    if(VisIt_VariableData_alloc(&hdl) == VISIT_OKAY)
      {
      VisIt_VariableData_setDataI(hdl, VISIT_OWNER_VISIT, 1, 1, iptr);
      VisIt_DomainList_setDomains(h, sim->par_size, hdl);
      }
    }
  return h;
}

/* Helper function for ProcessVisItCommand */
void BroadcastSlaveCommand(int *command, simulation_data *sim)
{
#ifdef PARALLEL
  MPI_Bcast(command, 1, MPI_INT, 0, sim->topocomm);
#endif
}

/* Callback involved in command communication. */
void SlaveProcessCallback(void *cbdata)
{
   simulation_data *sim = (simulation_data *)cbdata;
   int command = VISIT_COMMAND_PROCESS;
   BroadcastSlaveCommand(&command, sim);
}

/* Process commands from viewer on all processors. */
int ProcessVisItCommand(simulation_data *sim)
{
    int command;
    if (sim->par_rank==0)
    {  
        int success = VisItProcessEngineCommand();

        if (success == VISIT_OKAY)
        {
            command = VISIT_COMMAND_SUCCESS;
            BroadcastSlaveCommand(&command, sim);
            return 1;
        }
        else
        {
            command = VISIT_COMMAND_FAILURE;
            BroadcastSlaveCommand(&command, sim);
            return 0;
        }
    }
    else
    {
        /* Note: only through the SlaveProcessCallback callback
         * above can the rank 0 process send a VISIT_COMMAND_PROCESS
         * instruction to the non-rank 0 processes. */
        while (1)
        {
            BroadcastSlaveCommand(&command, sim);
            switch (command)
            {
            case VISIT_COMMAND_PROCESS:
                VisItProcessEngineCommand();
                break;
            case VISIT_COMMAND_SUCCESS:
                return 1;
            case VISIT_COMMAND_FAILURE:
                return 0;
            }
        }
    }
}

void Create_InSitu_Outputs(simulation_data *sim)
{
  int savedFile = 0, exportedFile = 0;
  VisItTimeStepChanged();
  VisItUpdatePlots();

  if(sim->iter%INCREMENT == 0)
    {
    if(sim->savingFiles)
    {
        char filename[100];
        sprintf(filename, "jacobi.%04d.png", sim->saveCounter);
        if(VisItSaveWindow(filename, 800, 800, VISIT_IMAGEFORMAT_PNG) == VISIT_OKAY)
        {
            savedFile = 1;
            if(sim->par_rank == 0)
                printf("Saved %s\n", filename);
        }
        else if(sim->par_rank == 0)
            printf("The image could not be saved to %s\n", filename);
    }

    if(sim->export)
    {
        char filename[100];
        visit_handle vars = VISIT_INVALID_HANDLE;
        VisIt_NameList_alloc(&vars);
        VisIt_NameList_addName(vars, "default");
#define TEST_VTK_OPTIONS 1
#ifdef TEST_VTK_OPTIONS
        {
            /* Create an option list that tells the VTK export to
             * blah blah blah.
             */
            visit_handle options = VISIT_INVALID_HANDLE;
            VisIt_OptionList_alloc(&options);
            VisIt_OptionList_setValueB(options, "Binary format", 1);
            VisIt_OptionList_setValueB(options, "XML format", 1);
            sprintf(filename, "updateplots_export%04d", sim->saveCounter);
            if(VisItExportDatabaseWithOptions(filename, "VTK_1.0", 
                                              vars, options) &&
               sim->par_rank == 0)
            {
                 printf("Exported %s\n", filename);
            }

            VisIt_OptionList_free(options);
        }
#else
        sprintf(filename, "updateplots_export%04d", sim->saveCounter);
        if(VisItExportDatabase(filename, "VTK_1.0", vars) &&
           sim->par_rank == 0)
        {
            printf("Exported %s\n", filename);
        }
#endif
        VisIt_NameList_free(vars);

        exportedFile = 1;
    }

    if(savedFile  || exportedFile)
        sim->saveCounter++;
    }
}

void ControlCommandCallback(const char *cmd, const char *args, void *cbdata)
{
    simulation_data *sim = (simulation_data *)cbdata;
    if(strcmp(cmd, "halt") == 0)
        sim->runMode = SIM_STOPPED;
    else if(strcmp(cmd, "step") == 0)
      {
      simulate_one_timestep(sim);
      Create_InSitu_Outputs(sim);
      }
    else if(strcmp(cmd, "restart") == 0)
      {
      set_initial_bc(sim);
      VisItTimeStepChanged();
      VisItUpdatePlots();
      }
    else if(strcmp(cmd, "run") == 0)
      sim->runMode = SIM_RUNNING;
    else if(strcmp(cmd, "update") == 0)
      {
      VisItTimeStepChanged();
      VisItUpdatePlots();
      }
}

/* This function is called when we need to install callback functions.
 */
void
SetupCallbacks(simulation_data *sim)
{
    VisItSetCommandCallback(ControlCommandCallback, (void*)sim);
    VisItSetSlaveProcessCallback2(SlaveProcessCallback, (void*)sim);

    VisItSetGetMetaData(SimGetMetaData, (void*)sim);
    VisItSetGetMesh(SimGetMesh, (void*)sim);
    //VisItSetGetCurve(SimGetCurve, (void*)sim);
    VisItSetGetVariable(SimGetVariable, (void*)sim);
#ifdef PARALLEL
    VisItSetGetDomainList(SimGetDomainList, (void*)sim);
#endif
}

